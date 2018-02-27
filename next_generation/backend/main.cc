

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
#include <epan/addr_resolv.h>
#include <epan/print.h>
#include <epan/epan_dissect.h>
#include <cfile.h>
#include "lib.h"
#include "libws_debug.h"
#include <slankdev/hexdump.h>

#include "wrapper.h"

int main(int argc, char** argv)
{
  init_process_policies();
  relinquish_special_privs_perm();
  init_progfile_dir(argv[0], main);
  wtap_init(true);
  epan_init(register_all_protocols, register_all_protocol_handoffs, NULL, NULL);

  capture_file cfile;
  cap_file_init(&cfile);
  const char* filename = "in.pcap";
  cfile.filename = (gchar*)filename;
  cfile.epan = cuishark_epan_new(&cfile);

  int err=0;
  gchar *err_info=nullptr;
  if (cf_open(&cfile, filename, WTAP_TYPE_AUTO, false, &err) != CF_OK) {
    exit(1);
  }

  epan_dissect_t* edt = epan_dissect_new(cfile.epan, true, true);
  printf("edt: %p\n", edt);

  while (true) {
    gint64 data_offset;
    bool ret = wtap_read(cfile.provider.wth, &err, &err_info, &data_offset);
    if (!ret) {
      printf("finish \n");
      break;
    }
    guint32 cum_bytes;
    frame_data fdlocal;
    cfile.count ++;
    guint32 framenum = cfile.count;
    printf("framenum: %u \n", framenum);
    wtap_rec* rec = wtap_get_rec(cfile.provider.wth);
    frame_data_init(&fdlocal, framenum, rec, data_offset, cum_bytes);

    frame_data_set_before_dissect(&fdlocal, &cfile.elapsed_time,
        &cfile.provider.ref, cfile.provider.prev_dis);
    cfile.provider.ref = &fdlocal;

    const guchar* pd = wtap_get_buf_ptr(cfile.provider.wth);
    epan_dissect_run(edt, cfile.cd_t, rec,
                   frame_tvbuff_new(&cfile.provider, &fdlocal, pd),
                   &fdlocal, NULL);

    print_stream_t* print_stream = print_stream_text_stdio_new(stdout);
    proto_tree_print(print_dissections_none, false, edt,
        nullptr, print_stream);

    /* print hex data */
    tvbuff_t* tvb = libwireshark::edt_get_tvb(edt);
    const uint8_t* ptr = libwireshark::tvb_get_ptr(tvb);
    size_t len = libwireshark::tvb_get_len(tvb);
    slankdev::hexdump(stdout, ptr, len);

    epan_dissect_reset(edt);
    frame_data_set_after_dissect(&fdlocal, &cum_bytes);
    frame_data_destroy(&fdlocal);
    printf("\n\n");
  }
  epan_dissect_free(edt);
}

