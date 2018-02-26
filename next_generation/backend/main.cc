

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
  {
    if (cf_open(&cfile, filename, WTAP_TYPE_AUTO, false, &err) != CF_OK) {
      exit(1);
      // epan_cleanup();
      // extcap_cleanup();
      // exit_status = INVALID_FILE;
      // goto clean_exit;
    }
  }

  gint64 data_offset;
  if (wtap_read(cfile.provider.wth, &err, &err_info, &data_offset)) {
    printf("slankdev\n");
  }
  printf("success\n");

  // epan_register_plugin_types();
  // scan_plugins();
  // wtap_register_plugin_types();
  // char* ret = init_progfile_dir(nullptr, nullptr);

#if 0
  bool ret = epan_init(register_all_protocols, register_all_protocol_handoffs, NULL, NULL);
  printf("%s\n", ret?"true":"false");
  if (!ret) {
    fprintf(stderr, "Error at epan_init\n");
    return 2;
  }
#endif
}

