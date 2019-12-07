#ifndef MODE_H
#define MODE_H

#include <Arduino.h>

class Mode {
  static Mode *current;

protected:
  bool presentation;

public:
  Mode() : presentation(false) {};
  static void Set(Mode *m);
  static Mode * Get() { return current; }

  virtual void Init() = 0;
  virtual void Update() = 0;
  virtual void Reset() = 0;
};

#endif
