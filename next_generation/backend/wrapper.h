
#pragma once
#include <epan/tvbuff.h>

namespace libwireshark {

inline size_t tvb_get_len(tvbuff_t* buf)
{
  size_t ret = tvb_captured_length(buf);
  return ret;
}

inline const uint8_t* tvb_get_ptr(tvbuff_t* buf)
{
  size_t len = tvb_get_len(buf);
  const uint8_t* ptr = tvb_get_ptr(buf, 0, len);
  return ptr;
}

inline tvbuff_t* edt_get_tvb(epan_dissect_t* edt)
{
  GSList* src_le = edt->pi.data_src;
  data_source* src = (data_source*)src_le->data;
  tvbuff_t* tvb = get_data_source_tvb(src);
  return tvb;
}


} /* libwireshark */
