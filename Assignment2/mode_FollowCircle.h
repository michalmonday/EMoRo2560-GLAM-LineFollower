#ifndef FOLLOWCIRCLE_H
#define FOLLOWCIRCLE_H

#include "mode.h"
#include "Assignment2.h"

class FollowCircle : public Mode {
  bool last_iteration_had_left_turn, last_iteration_had_right_turn, mario_sound;
  int number_of_left_turns, number_of_right_turns, direction;

  float GetOptimalSharpness(int dir);
  void OutputTurnsCounts();
  void DirectionFoundSoundEffect();
  void SetDirection(int dir);
  void SlightTurnTechnique();
public:
  FollowCircle() : direction(NULL), number_of_left_turns(NULL), number_of_right_turns(NULL), last_iteration_had_left_turn(false), last_iteration_had_right_turn(false), mario_sound(true) {}
  
  void Init() override;
  void Update() override;
  void Reset() override;

  void SetClockwise() { direction = CLOCKWISE; }
  void SetAntiClockwise() { direction = ANTI_CLOCKWISE; }
  void OutputDirection();
};

#endif
