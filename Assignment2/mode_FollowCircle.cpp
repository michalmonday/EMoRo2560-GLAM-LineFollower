#include "includes.h"

#define DIR_DETECTION_TURN_THRESHOLD 2

void FollowCircle::Init() { 
  Display::Msg("Circle track", "following.");
  drive.SetSpeed(300);
}

void FollowCircle::Update() {
  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  Serial.println(String(l_) +","+ String(c) +","+ String(r));
  if (!l_ && c && !r) {
    if (!direction) {
      drive.Forward();
    }
    else {
      if (direction == CLOCKWISE) {
        drive.Right(false, false, 2.0);
      }
      else {
        drive.Left(false, false, 2.0);
      }
    }
    
    last_iteration_had_right_turn = false;
    last_iteration_had_left_turn = false;
  }

  if (l_ && !r) {
    drive.Left(false, false, direction ? GetOptimalSharpness(ANTI_CLOCKWISE) : 2.0);

    if (!last_iteration_had_left_turn){
      number_of_left_turns++;
      last_iteration_had_left_turn = true;
      last_iteration_had_right_turn = false;
    }
  }

  
  if (!l_ && r) {
    drive.Right(false, false, direction ? GetOptimalSharpness(CLOCKWISE) : 2.0);

    if (!last_iteration_had_right_turn) {
      number_of_right_turns++;
      last_iteration_had_right_turn = true;
      last_iteration_had_left_turn = false;
    }
  }

  if (!direction) {
    if (number_of_left_turns >= DIR_DETECTION_TURN_THRESHOLD) {
      direction = ANTI_CLOCKWISE;
      BT_SERIAL.println("The number of left turns exceeded threshold. Anti-clockwise direction was detected.");
      drive.SetSpeed(1000);
    }

    if (number_of_right_turns >= DIR_DETECTION_TURN_THRESHOLD) {
      direction = CLOCKWISE;
      BT_SERIAL.println("The number of right turns exceeded threshold. Clockwise direction was detected.");
      drive.SetSpeed(1000);
    }
  }
}

void FollowCircle::Reset() {
  number_of_left_turns = number_of_right_turns = NULL; 
  last_iteration_had_left_turn = last_iteration_had_right_turn = false;
  direction = NULL;
}

float FollowCircle::GetOptimalSharpness(int dir) {
  return direction == dir ? 1.5 : 5.0;
}
