
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "StatusLine.h"
#include "TextPane.h"
#include "ToggleListPane.h"

#include "PacketListPane.h"
#include "Cuishark.h"

extern CuisharkInfo info;

enum FocusState {
  PANE1,
  PANE2,
  PANE3,
};

class TuiFrontend {
  void draw_panes_headers();
  void resize(PaneInterface *pane);
  void resize(Statusline* sline);

 public:
  static FocusState fstate;
  PacketListPane  pane1;
  ToggleListPane  pane2;
  TextPane        pane3;
  Statusline      sline;

 public:
  TuiFrontend();
  virtual ~TuiFrontend();

  static void init();
  void refresh();

  void key_input(int c);
  void packet_input(const void* p, size_t l, uint64_t t);

  void resize();

  void focuse_sw();
  FocusState get_state() const { return fstate; }
};



