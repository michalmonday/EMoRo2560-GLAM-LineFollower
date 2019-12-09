#ifndef FOLLOWRECT_H
#define FOLLOWRECT_H

#include <Arduino.h>
#include "mode.h"

#define SENSORS_OFFSET         25.0

/*  new batteries provide around 8.95V (read using float-ReadPowerSupply function)
    old batteries provide around 6.6V
    The car drives faster with new batteries.
    It was observed that a car with batteries almost running out (6.6V when checked with ReadPowerSupply)
    drives around 10% slower than a car with new batteries (8.95V when checked with ReadPowerSupply).  
    The exact optimal timings that were observed are supplied to the "map" function in the AssignEstimatedTravelTime method. */

/*   Setting these worked well with low battery level (6.6V) (before compensation method was implemented)
     #define LONG_SIDE_TRAVEL_TIME  3520
     #define SHORT_SIDE_TRAVEL_TIME 2300

   Setting that worked well with high battery level (8.95V) (before compensation method was implemented)
     #define LONG_SIDE_TRAVEL_TIME  3168 // decreased by %10
     #define SHORT_SIDE_TRAVEL_TIME 2070      */

class FollowRect : public Mode {
  unsigned long start_time, travel_time, is_near_end_threshold_time;
  float voltage_adjustment_mult, following_sharpness_div, power_supply_significance;
  bool is_near_end, reached_end, is_first_turn; 
  int side_len, long_side_len, short_side_len, turn_direction, rebound_size;

  void AssignEstimatedTravelTime(); 
  void AdjustTravelTimeByBatteryLevel();
  
public:

  FollowRect(int ldl, int sdl) : long_side_len(ldl), short_side_len(sdl), rebound_size(24), power_supply_significance(0.12) {}
  void Init() override;
  void Update() override;
  void Reset() override;

  void SetLongSideLen(int len);
  void SetShortSideLen(int len);
  void IncreaseFollowingSharpness();
  void DecreaseFollowingSharpness();
  void IncreaseReboundSize(); 
  void DecreaseReboundSize();
  void IncreasePowerSupplySignificance();
  void DecreasePowerSupplySignificance();

  void OutputAllSettings();
};



#endif
