#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

namespace Display {
  extern void Msg(String msg);
  extern void Msg(String msg_row_1, String msg_row_2);
  extern void Msg(String top_left, String top_right, String bottom_left, String bottom_right);
}

#endif
