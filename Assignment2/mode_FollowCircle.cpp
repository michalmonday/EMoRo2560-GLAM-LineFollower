#include "includes.h"

#define DIR_DETECTION_TURN_THRESHOLD 6

void FollowCircle::Init() { 
  Display::Msg("Circle track", "following.");
  drive.SetSpeed(300);
}

void FollowCircle::Update() {
  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  Serial.println(String(l_) +","+ String(c) +","+ String(r));
  if (!l_ && c && !r)
    drive.Forward();

  if (l_ && !r) {
    drive.Left(false, false, direction ? GetOptimalSharpness(ANTI_CLOCKWISE) : 2.0);
    number_of_left_turns++;
  }
  
  if (!l_ && r) {
    drive.Right(false, false, direction ? GetOptimalSharpness(CLOCKWISE) : 2.0);
    number_of_right_turns++;
  }

  if (!direction) {
    if (number_of_left_turns > DIR_DETECTION_TURN_THRESHOLD) {
      direction = ANTI_CLOCKWISE;
      BT_SERIAL.println("The number of left turns exceeded threshold. Anti-clockwise direction was detected.");
      drive.SetSpeed(1000);
    }

    if (number_of_right_turns > DIR_DETECTION_TURN_THRESHOLD) {
      direction = CLOCKWISE;
      BT_SERIAL.println("The number of right turns exceeded threshold. Clockwise direction was detected.");
      drive.SetSpeed(1000);
    }
  }
}

void FollowCircle::Reset() {
  number_of_left_turns = number_of_right_turns = NULL;  
  direction = NULL;
}

float FollowCircle::GetOptimalSharpness(int dir) {
  return direction == dir ? 1.5 : 15;
}
