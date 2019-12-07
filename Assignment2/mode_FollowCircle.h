#ifndef FOLLOWCIRCLE_H
#define FOLLOWCIRCLE_H

#include "mode.h"
#include "Assignment2.h"

class FollowCircle : public Mode {
  bool last_iteration_had_left_turn, last_iteration_had_right_turn;
  int number_of_left_turns, number_of_right_turns, direction;

  float GetOptimalSharpness(int dir);
public:
  FollowCircle() : direction(NULL), number_of_left_turns(NULL), number_of_right_turns(NULL), last_iteration_had_left_turn(false), last_iteration_had_right_turn(false) {}
  
  void Init() override;
  void Update() override;
  void Reset() override;

  void SetClockwise() { direction = CLOCKWISE; }
  void SetAntiClockwise() { direction = ANTI_CLOCKWISE; }
};

#endif
