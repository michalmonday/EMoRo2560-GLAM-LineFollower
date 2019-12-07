#include "display.h"

namespace Display { 
  void Msg(String msg) {
    Lcd.clear();
    Lcd.locate(0,0); Lcd.print(msg); 
  }
  
  void Msg(String msg_row_1, String msg_row_2) {
    Lcd.clear();
    Lcd.locate(0,0); Lcd.print(msg_row_1); 
    Lcd.locate(1,0); Lcd.print(msg_row_2); 
  }
  
  void Msg(String top_left, String top_right, String bottom_left, String bottom_right) {
    Lcd.clear();
    Lcd.locate(0,0); Lcd.print(top_left); 
    int pos = 16 - top_right.length();
    Lcd.locate(0, pos < 8 ? 8 : pos); Lcd.print(top_right); 
    Lcd.locate(1,0); Lcd.print(bottom_left); 
    pos = 16 - bottom_right.length();
    Lcd.locate(1, pos < 8 ? 8 : pos); Lcd.print(bottom_right); 
  }
}
