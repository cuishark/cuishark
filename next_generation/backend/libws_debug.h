
#pragma once
#include <cfile.h>

inline const char* boolstr(bool b) { return b?"true":"false"; }

inline void print_capture_file(const capture_file* cfile)
{
  printf("epan_t      *epan                   : %p \n", cfile->epan);
  printf("file_state   state                  : none \n");
  printf("gchar       *filename               : %s \n", cfile->filename);
  printf("gchar       *source                 : %s \n", cfile->source  );
  printf("gboolean     is_tempfile            : %s \n", boolstr(cfile->is_tempfile    ));
  printf("gboolean     unsaved_changes        : %s \n", boolstr(cfile->unsaved_changes));
  printf("gboolean     stop_flag              : %s \n", boolstr(cfile->stop_flag      ));
  printf("gint64       f_datalen              : %lu \n", cfile->f_datalen );
  printf("guint16      cd_t                   : %u \n", cfile->cd_t      ) ;
  printf("unsigned int open_type              : %u \n", cfile->open_type ) ;
  printf("gboolean     iscompressed           : %s \n", boolstr(cfile->iscompressed));
  printf("int          lnk_t                  : %d \n", cfile->lnk_t);
  printf("GArray      *linktypes              : none \n");
  printf("guint32      count                  : %u \n", cfile->count                );
  printf("guint64      packet_comment_count   : %lu \n", cfile->packet_comment_count );
  printf("guint32      displayed_count        : %u \n", cfile->displayed_count      );
  printf("guint32      marked_count           : %u \n", cfile->marked_count         );
  printf("guint32      ignored_count          : %u \n", cfile->ignored_count        );
  printf("guint32      ref_time_count         : %u \n", cfile->ref_time_count       );
  printf("gboolean     drops_known            : %s   \n", boolstr(cfile->drops_known));
  printf("guint32      drops                  : %u   \n", cfile->drops            );
  printf("nstime_t     elapsed_time           : none \n");
  printf("int          snap                   : %d   \n", cfile->snap             );
  printf("dfilter_t   *rfcode                 : %p   \n", cfile->rfcode           );
  printf("dfilter_t   *dfcode                 : %p   \n", cfile->dfcode           );
  printf("gchar       *dfilter                : %s   \n", cfile->dfilter          );
  printf("gboolean     redissecting           : %s   \n", boolstr(cfile->redissecting )    );
  printf("/* search */                               \n");
  printf("gchar       *sfilter                : %s   \n", cfile->sfilter          );
  printf("gboolean     hex                    : %s   \n", boolstr(cfile->hex             ) );
  printf("gboolean     string                 : %s   \n", boolstr(cfile->string          ) );
  printf("gboolean     summary_data           : %s   \n", boolstr(cfile->summary_data    ) );
  printf("gboolean     decode_data            : %s   \n", boolstr(cfile->decode_data     ) );
  printf("gboolean     packet_data            : %s   \n", boolstr(cfile->packet_data     ) );
  printf("guint32      search_pos             : %u   \n", cfile->search_pos       );
  printf("guint32      search_len             : %u   \n", cfile->search_len       );
  printf("gboolean     case_type              : %s   \n", boolstr(cfile->case_type      )  );
  printf("GRegex      *regex                  : none \n");
  printf("search_charset_t scs_type           : none \n");
  printf("search_direction dir                : none \n");
  printf("gboolean     search_in_progress     : %s   \n", boolstr(cfile->search_in_progress)  );
  printf("/* packet data */                          \n");
  printf("wtap_rec     rec                    : none \n");
  printf("Buffer       buf                    : none \n");
  printf("/* packet provider */                      \n");
  printf("struct packet_provider_data provider: none \n");
  printf("/* frames */                               \n");
  printf("guint32      first_displayed        : %u   \n", cfile->first_displayed   );
  printf("guint32      last_displayed         : %u   \n", cfile->last_displayed    );
  printf("column_info  cinfo                  : none \n");
  printf("gboolean     columns_changed        : %s   \n", boolstr(cfile->columns_changed)    );
  printf("frame_data  *current_frame          : %p   \n", cfile->current_frame      );
  printf("gint         current_row            : %d   \n", cfile->current_row        );
  printf("epan_dissect_t *edt                 : %p   \n", cfile->edt             );
  printf("field_info  *finfo_selected         : %p   \n", cfile->finfo_selected     );
  printf("gpointer     window                 : none \n");
  printf("gulong       computed_elapsed       : %lu  \n", cfile->computed_elapsed );
  printf("guint32      cum_bytes              : %u   \n", cfile->cum_bytes         );
}


