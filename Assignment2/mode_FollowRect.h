#ifndef FOLLOWRECT_H
#define FOLLOWRECT_H

#include <Arduino.h>
#include "mode.h"

#define SENSORS_OFFSET         20.0

/*  new batteries provide around 8.95V (read using float-ReadPowerSupply function)
    old batteries provide around 6.6V
    The car drives faster with new batteries.
    It was observed that a car with batteries almost running out (6.6V when checked with ReadPowerSupply)
    drives around 10% slower than a car with new batteries (8.95VV when checked with ReadPowerSupply).  */
#define LONG_SIDE_TRAVEL_TIME  3520 
#define SHORT_SIDE_TRAVEL_TIME 2300

/* ReadPowerSupply has to be read once at the begining, 
   reading it during operation sometimes results in lower readings, 
   it's unstable.

   Setting that worked well with low battery level (6.6V) (before compensation method was implemented)
     #define LONG_SIDE_TRAVEL_TIME  3520
     #define SHORT_SIDE_TRAVEL_TIME 2300

   Setting that worked well with high battery level (8.95V) (before compensation method was implemented)
     #define LONG_SIDE_TRAVEL_TIME  3168 // decreased by %10
     #define SHORT_SIDE_TRAVEL_TIME 2070      */

class FollowRect : public Mode {
  unsigned long start_time, travel_time, is_near_end_threshold_time;
  float voltage_adjustment_mult;
  bool is_near_end, reached_end, is_first_turn; 
  int side_len, long_side_len, short_side_len, turn_direction;

  void AssignEstimatedTravelTime(); 
  void AdjustTravelTimeByBatteryLevel();
  
public:

  FollowRect(int ldl, int sdl) : long_side_len(ldl), short_side_len(sdl) {}
  void Init() override;
  void Update() override;
  void Reset() override;

  void SetLongSideLen(int len) { long_side_len = len; }
  void SetShortSideLen(int len) { short_side_len = len; }
};



#endif
