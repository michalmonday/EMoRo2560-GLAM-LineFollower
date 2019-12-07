#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define BT_SERIAL Serial1

#include <Arduino.h>

class Bt {
  bool debug;
public:
  Bt(char name_[16], char pin[16], bool debug = false);
  void CheckProtocol(String s);
  void Update();
};

#endif
