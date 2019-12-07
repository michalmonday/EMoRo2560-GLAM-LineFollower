#include "includes.h"

void FollowCircle::Init() { 
  Display::Msg("Circle track", "following.");
}

void FollowCircle::Update() {
  bool l_,c,r;
  tracker_sensor->GetAll(l_,c,r);

  Serial.println(String(l_) +","+ String(c) +","+ String(r));
  if (!l_ && c && !r)
    drive.Forward();

  if (l_ && !r)
    drive.Left(false, false, direction == CLOCKWISE ? 15 : 1.5); // if center detects black line then soft turn, if center detects no black line then sharp turn

  if (!l_ && r)
    drive.Right(false, false,  direction == CLOCKWISE ? 1.5 : 15); //c ? 2 : 1.7
}

void FollowCircle::Reset() {}
