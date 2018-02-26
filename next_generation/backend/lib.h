
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <glib.h>
#include <epan/epan.h>
#include <wiretap/wtap.h>
#include <wsutil/plugins.h>
#include <wsutil/filesystem.h>
#include <wsutil/privileges.h>
#include <epan/packet.h>
#include <epan/tvbuff.h>
#include <epan/tvbuff-int.h>

#include <cfile.h>

void cap_file_init(capture_file *cf)
{
  /* Initialize the capture file struct */
  memset(cf, 0, sizeof(capture_file));
}

const char *
cap_file_provider_get_interface_description(struct packet_provider_data *prov, guint32 interface_id)
{
  wtapng_iface_descriptions_t *idb_info;
  wtap_block_t wtapng_if_descr = NULL;
  char* interface_name;

  idb_info = wtap_file_get_idb_info(prov->wth);

  if (interface_id < idb_info->interface_data->len)
    wtapng_if_descr = g_array_index(idb_info->interface_data, wtap_block_t, interface_id);

  g_free(idb_info);

  if (wtapng_if_descr) {
    if (wtap_block_get_string_option_value(wtapng_if_descr, OPT_IDB_DESCR, &interface_name) == WTAP_OPTTYPE_SUCCESS)
      return interface_name;
  }
  return NULL;
}

const char *
cap_file_provider_get_interface_name(struct packet_provider_data *prov, guint32 interface_id)
{
  wtapng_iface_descriptions_t *idb_info;
  wtap_block_t wtapng_if_descr = NULL;
  char* interface_name;

  idb_info = wtap_file_get_idb_info(prov->wth);

  if (interface_id < idb_info->interface_data->len)
    wtapng_if_descr = g_array_index(idb_info->interface_data, wtap_block_t, interface_id);

  g_free(idb_info);

  if (wtapng_if_descr) {
    if (wtap_block_get_string_option_value(wtapng_if_descr, OPT_IDB_NAME, &interface_name) == WTAP_OPTTYPE_SUCCESS)
      return interface_name;
    if (wtap_block_get_string_option_value(wtapng_if_descr, OPT_IDB_DESCR, &interface_name) == WTAP_OPTTYPE_SUCCESS)
      return interface_name;
  }
  return "unknown";
}

static const nstime_t *
tshark_get_frame_ts(struct packet_provider_data *prov, guint32 frame_num)
{
  if (prov->ref && prov->ref->num == frame_num)
    return &prov->ref->abs_ts;
  if (prov->prev_dis && prov->prev_dis->num == frame_num)
    return &prov->prev_dis->abs_ts;
  if (prov->prev_cap && prov->prev_cap->num == frame_num)
    return &prov->prev_cap->abs_ts;
  if (prov->frames) {
     frame_data *fd = frame_data_sequence_find(prov->frames, frame_num);
     return (fd) ? &fd->abs_ts : NULL;
  }
  return NULL;
}

static epan_t* cuishark_epan_new(capture_file *cf)
{
  static const struct packet_provider_funcs funcs = {
    tshark_get_frame_ts,
    cap_file_provider_get_interface_name,
    cap_file_provider_get_interface_description,
    NULL,
  };
  return epan_new(&cf->provider, &funcs);
}

typedef enum {
    CF_OK,      /**< operation succeeded */
    CF_ERROR    /**< operation got an error (function may provide err with details) */
} cf_status_t;

cf_status_t
cf_open(capture_file *cf, const char *fname, unsigned int type, gboolean is_tempfile, int *err)
{
  wtap  *wth;
  gchar *err_info;

  wth = wtap_open_offline(fname, type, err, &err_info, true);
  if (wth == NULL)
    goto fail;

  /* The open succeeded.  Fill in the information for this file. */

  /* Create new epan session for dissection. */
  epan_free(cf->epan);
  cf->epan = cuishark_epan_new(cf);

  cf->provider.wth = wth;
  cf->f_datalen = 0; /* not used, but set it anyway */

  /* Set the file name because we need it to set the follow stream filter.
     XXX - is that still true?  We need it for other reasons, though,
     in any case. */
  cf->filename = g_strdup(fname);

  /* Indicate whether it's a permanent or temporary file. */
  cf->is_tempfile = is_tempfile;

  /* No user changes yet. */
  cf->unsaved_changes = FALSE;

  cf->cd_t      = wtap_file_type_subtype(cf->provider.wth);
  cf->open_type = type;
  cf->count     = 0;
  cf->drops_known = FALSE;
  cf->drops     = 0;
  cf->snap      = wtap_snapshot_length(cf->provider.wth);
  nstime_set_zero(&cf->elapsed_time);
  cf->provider.ref = NULL;
  cf->provider.prev_dis = NULL;
  cf->provider.prev_cap = NULL;

  cf->state = FILE_READ_IN_PROGRESS;

  wtap_set_cb_new_ipv4(cf->provider.wth, add_ipv4_name);
  wtap_set_cb_new_ipv6(cf->provider.wth, (wtap_new_ipv6_callback_t) add_ipv6_name);

  return CF_OK;

fail:
  // cfile_open_failure_message("TShark", fname, *err, err_info);
  return CF_ERROR;
}

static GPtrArray *buffer_cache = NULL;
struct tvb_frame {
	struct tvbuff tvb;
	Buffer *buf;         /* Packet data */
	const struct packet_provider_data *prov;	/* provider of packet information */
	gint64 file_off;     /**< File offset */
	guint offset;
};

static void
frame_free(tvbuff_t *tvb)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;

	if (frame_tvb->buf) {
		ws_buffer_free(frame_tvb->buf);
		g_ptr_array_add(buffer_cache, frame_tvb->buf);
	}
}

static guint
frame_offset(const tvbuff_t *tvb _U_, const guint counter)
{
	/* XXX: frame_tvb->offset */
	return counter;
}

static gboolean
frame_read(struct tvb_frame *frame_tvb, wtap_rec *rec, Buffer *buf)
{
	int    err;
	gchar *err_info;

	/* XXX, what if phdr->caplen isn't equal to
	 * frame_tvb->tvb.length + frame_tvb->offset?
	 */
	if (!wtap_seek_read(frame_tvb->prov->wth, frame_tvb->file_off, rec, buf, &err, &err_info)) {
		/* XXX - report error! */
		switch (err) {
			case WTAP_ERR_BAD_FILE:
				g_free(err_info);
				break;
		}
		return FALSE;
	}
	return TRUE;
}


static void
frame_cache(struct tvb_frame *frame_tvb)
{
	wtap_rec rec; /* Record metadata */

	wtap_rec_init(&rec);

	if (frame_tvb->buf == NULL) {
		if G_UNLIKELY(!buffer_cache) buffer_cache = g_ptr_array_sized_new(1024);

		if (buffer_cache->len > 0) {
			frame_tvb->buf = (struct Buffer *) g_ptr_array_remove_index(buffer_cache, buffer_cache->len - 1);
		} else {
			frame_tvb->buf = (struct Buffer *) g_malloc(sizeof(struct Buffer));
		}

		ws_buffer_init(frame_tvb->buf, frame_tvb->tvb.length + frame_tvb->offset);

		if (!frame_read(frame_tvb, &rec, frame_tvb->buf))
			{ /* TODO: THROW(???); */ }
	}

	frame_tvb->tvb.real_data = ws_buffer_start_ptr(frame_tvb->buf) + frame_tvb->offset;

	wtap_rec_cleanup(&rec);
}


static const guint8 *
frame_get_ptr(tvbuff_t *tvb, guint abs_offset, guint abs_length _U_)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;

	frame_cache(frame_tvb);

	return tvb->real_data + abs_offset;
}

static void *
frame_memcpy(tvbuff_t *tvb, void *target, guint abs_offset, guint abs_length)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;

	frame_cache(frame_tvb);

	return memcpy(target, tvb->real_data + abs_offset, abs_length);
}

static gint
frame_find_guint8(tvbuff_t *tvb, guint abs_offset, guint limit, guint8 needle)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;
	const guint8 *result;

	frame_cache(frame_tvb);

	result = (const guint8 *)memchr(tvb->real_data + abs_offset, needle, limit);
	if (result)
		return (gint) (result - tvb->real_data);
	else
		return -1;
}

static gint
frame_pbrk_guint8(tvbuff_t *tvb, guint abs_offset, guint limit, const ws_mempbrk_pattern* pattern, guchar *found_needle)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;

	frame_cache(frame_tvb);

	return tvb_ws_mempbrk_pattern_guint8(tvb, abs_offset, limit, pattern, found_needle);
}

static tvbuff_t * frame_clone(tvbuff_t *tvb, guint abs_offset, guint abs_length);

static const struct tvb_ops tvb_frame_ops = {
	sizeof(struct tvb_frame), /* size */
	frame_free,           /* free */
	frame_offset,         /* offset */
	frame_get_ptr,        /* get_ptr */
	frame_memcpy,         /* memcpy */
	frame_find_guint8,    /* find_guint8 */
	frame_pbrk_guint8,    /* pbrk_guint8 */
	frame_clone,          /* clone */
};

tvbuff_t *
tvb_new(const struct tvb_ops *ops)
{
	tvbuff_t *tvb;
	gsize     size = ops->tvb_size;

	g_assert(size >= sizeof(*tvb));

	tvb = (tvbuff_t *) g_slice_alloc(size);

	tvb->next	     = NULL;
	tvb->ops	     = ops;
	tvb->initialized     = FALSE;
	tvb->flags	     = 0;
	tvb->length	     = 0;
	tvb->reported_length = 0;
	tvb->real_data	     = NULL;
	tvb->raw_offset	     = -1;
	tvb->ds_tvb	     = NULL;

	return tvb;
}

static tvbuff_t *
frame_clone(tvbuff_t *tvb, guint abs_offset, guint abs_length)
{
	struct tvb_frame *frame_tvb = (struct tvb_frame *) tvb;

	tvbuff_t *cloned_tvb;
	struct tvb_frame *cloned_frame_tvb;

	/* file not seekable */
	if (!frame_tvb->prov)
		return NULL;

	abs_offset += frame_tvb->offset;

	cloned_tvb = tvb_new(&tvb_frame_ops);

	/* data will be read when needed */
	cloned_tvb->real_data       = NULL;
	cloned_tvb->length          = abs_length;
	cloned_tvb->reported_length = abs_length; /* XXX? */
	cloned_tvb->initialized     = TRUE;

	/*
	 * This is the top-level real tvbuff for this data source,
	 * so its data source tvbuff is itself.
	 */
	cloned_tvb->ds_tvb = cloned_tvb;

	cloned_frame_tvb = (struct tvb_frame *) cloned_tvb;
	cloned_frame_tvb->prov = frame_tvb->prov;
	cloned_frame_tvb->file_off = frame_tvb->file_off;
	cloned_frame_tvb->offset = abs_offset;
	cloned_frame_tvb->buf = NULL;

	return cloned_tvb;
}




tvbuff_t *
frame_tvbuff_new(const struct packet_provider_data *prov, const frame_data *fd,
    const guint8 *buf)
{
	struct tvb_frame *frame_tvb;
	tvbuff_t *tvb;

	tvb = tvb_new(&tvb_frame_ops);

	/*
	 * XXX - currently, the length arguments in
	 * tvbuff structure are signed, but the captured
	 * and reported length values are unsigned; this means
	 * that length values > 2^31 - 1 will appear as
	 * negative lengths
	 *
	 * Captured length values that large will already
	 * have been filtered out by the Wiretap modules
	 * (the file will be reported as corrupted), to
	 * avoid trying to allocate large chunks of data.
	 *
	 * Reported length values will not have been
	 * filtered out, and should not be filtered out,
	 * as those lengths are not necessarily invalid.
	 *
	 * For now, we clip the reported length at G_MAXINT
	 *
	 * (XXX, is this still a problem?) There was an exception when we call
	 * tvb_new_real_data() now there's no one
	 */

	tvb->real_data       = buf;
	tvb->length          = fd->cap_len;
	tvb->reported_length = fd->pkt_len > G_MAXINT ? G_MAXINT : fd->pkt_len;
	tvb->initialized     = TRUE;

	/*
	 * This is the top-level real tvbuff for this data source,
	 * so its data source tvbuff is itself.
	 */
	tvb->ds_tvb = tvb;

	frame_tvb = (struct tvb_frame *) tvb;

	/* XXX, wtap_can_seek() */
	// if (prov->wth && prov->wth->random_fh) {
	// 	frame_tvb->prov = prov;
	// 	frame_tvb->file_off = fd->file_off;
	// 	frame_tvb->offset = 0;
	// } else
  frame_tvb->prov = NULL;

	frame_tvb->buf = NULL;

	return tvb;
}
