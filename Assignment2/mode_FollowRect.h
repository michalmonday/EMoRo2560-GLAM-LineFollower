#ifndef FOLLOWRECT_H
#define FOLLOWRECT_H

#include <Arduino.h>
#include "mode.h"

#define LONG_SIDE_LINE_LEN     70.0
#define SHORT_SIDE_LINE_LEN    42.0
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
  bool is_side_long = true;
  unsigned long start_time = NULL;
  unsigned long travel_time = NULL; 
  float side_len = LONG_SIDE_LINE_LEN;
  unsigned long is_near_end_threshold_time = NULL;
  float voltage_adjustment_mult = NULL;
  int turn_direction = NULL;
  bool is_first_turn = true;
public:
  void Init() override;
  void Update() override;
  void Reset() override;
};

#endif
