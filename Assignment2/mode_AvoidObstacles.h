#ifndef AVOIDOBSTACLES_H
#define AVOIDOBSTACLES_H

#include "mode.h"

class AvoidObstacles : public Mode {
  double FindBestAngle();

  void PlayDetectionSound();
public:
  void Init() override;
  void Update() override;
  void Reset() override;
};

#endif
