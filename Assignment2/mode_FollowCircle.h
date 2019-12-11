#ifndef FOLLOWCIRCLE_H
#define FOLLOWCIRCLE_H

#include "mode.h"
#include "Assignment2.h"

class FollowCircle : public Mode {
  bool last_iteration_had_left_turn, last_iteration_had_right_turn, mario_sound;
  int number_of_left_turns, number_of_right_turns, direction;
  
  float last_turn_sharpness, prev_turn_sharpness;
  unsigned long last_turn_change_time;
  unsigned long smoothing_period;

  float in_turn_sharpness;
  float slight_turn_technique_sharpness;
  float out_turn_sharpness;
  
  void OutputTurnsCounts();
  void DirectionFoundSoundEffect();
  void SetDirection(int dir);
  void SlightTurnTechnique();

  float SmoothenSharpness(float s);
  void StorePreviousSharpness(float s);
public:
  FollowCircle() : direction(NULL), number_of_left_turns(NULL), number_of_right_turns(NULL), last_iteration_had_left_turn(false), last_iteration_had_right_turn(false), mario_sound(false),
    in_turn_sharpness(1.6), slight_turn_technique_sharpness(2.0), out_turn_sharpness(2.3), last_turn_sharpness(2.0), prev_turn_sharpness(2.0), last_turn_change_time(0), smoothing_period(150) {}
  
  void Init() override;
  void Update() override;
  void Reset() override;

  void SetClockwise() { direction = CLOCKWISE; }
  void SetAntiClockwise() { direction = ANTI_CLOCKWISE; }
  void OutputDirection();

  void IncreaseSmoothness();
  void DecreaseSmoothness();
};

#endif
