

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


int main(int argc, char** argv)
{
  wtap_init();
  register_all_wiretap_modules();

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

