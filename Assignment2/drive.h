#ifndef DRIVE_H
#define DRIVE_H

#include <Arduino.h>

class Drive {
private: 
  int speed_;
  
  void Rebound(bool right, int size_);
  
public:
  Drive();
  void SetSpeed(int s);

  void WheelsServosWrite(int left_val, int right_val);

  /* Set of functions that mainly call WheelsServosWrite with correct values. */
  void Stop();
  void Forward();
  void Backward();
  void Left(bool sharp = false, bool back = false, float divider = 2.0);
  void Right(bool sharp = false, bool back = false, float divider = 2.0);

  /* Function that is different from "Left/Right" functions.
     It waits until the turn is done, and allows to stop the turn if the line is recognized under the 3-sensors module. */
  bool Turn(int deg, bool stop_at_line = false, int rebound_size = 20);

  /* Gyroscope sensor is inaccurate, the Turn function (based on time estimate) is better for that. */
  void TurnAbsolute_GyroBased(double angle, bool stop_at_line = false);
  void Turn_GyroBased(double angle, bool stop_at_line = false);
};

/*  "Externing" the variable this way will allow to use the object in all the files that import this file.  */
extern Drive drive;

#endif
