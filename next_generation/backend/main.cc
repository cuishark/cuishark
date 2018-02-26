

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
#include <cfile.h>
#include "lib.h"


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

  gint64 data_offset;
  if (wtap_read(cfile.provider.wth, &err, &err_info, &data_offset)) {

    guint32 cum_bytes;
    frame_data fdlocal;
    guint32 framenum = cfile.count + 1;
    wtap_rec* rec = wtap_get_rec(cfile.provider.wth);
    frame_data_init(&fdlocal, framenum, rec, data_offset, cum_bytes);

    epan_dissect_t* edt = epan_dissect_new(cfile.epan, true, true);
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

    frame_data_set_after_dissect(&fdlocal, &cum_bytes);
    frame_data_destroy(&fdlocal);
    printf("success\n");
    return 0;
  }
  printf("false\n");
  return 1;
}

