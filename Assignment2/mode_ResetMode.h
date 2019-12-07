#ifndef RESETMODE_H
#define RESETMODE_H

#include "mode.h"

class ResetMode : public Mode {
public:
  void Init() override;
  void Update() override;
  void Reset() override;
};

#endif
