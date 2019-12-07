#include "tracker_sensor.h"

TrackerSensor *tracker_sensor;

TrackerSensor::TrackerSensor(int l, int c, int r)
  : left_pin(l), center_pin(c), right_pin(r)
{ 
  pinMode(l, INPUT_PULLUP);  // left                                 
  pinMode(c, INPUT_PULLUP);  // center
  pinMode(r, INPUT_PULLUP);  // right  
}

/*  Read all 3 sensors from the board. Arguments passed by reference work similar to pointers in plain C.
    It allows setting 3 "return values" at once. */
void TrackerSensor::GetAll(bool &l, bool &c, bool &r) {
  l = !digitalRead(left_pin); 
  c = !digitalRead(center_pin); 
  r = !digitalRead(right_pin);
}

/*  Function that allows convenient printing of sensor state. Useful for debugging.  */
String TrackerSensor::StrValues() {
  bool l_,c,r;   GetAll(l_,c,r);
  return "tracker_sensor = " + String(l_) + " " + String(c) + " " + String(r);
}

/*  Check if the front sensor is directly over the line.  */
bool TrackerSensor::IsOnTheLine() {
  bool l_,c,r;    GetAll(l_,c,r);
  return (!l_ && c && !r);
}
