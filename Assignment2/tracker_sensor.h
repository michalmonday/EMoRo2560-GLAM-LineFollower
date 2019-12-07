#ifndef TRACKERSENSOR_H
#define TRACKERSENSOR_H

#include <Arduino.h>

class TrackerSensor {
  int left_pin;
  int center_pin;
  int right_pin;

public:
  TrackerSensor(int l_pin, int c_pin, int r_pin);  

  void GetAll(bool &l_state, bool &c_state, bool &r_state); 
  bool IsOnTheLine();
  String StrValues();
};

/*  "Externing" the variable this way will allow to use the object in all the files that import this file.  */
extern TrackerSensor *tracker_sensor;

#endif
