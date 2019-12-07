#include "includes.h"

void ResetMode::Init() {
  drive.Stop();
  Display::Msg("1.Obst", "2.Circle", "3.Square", "4.Reset");
}

void ResetMode::Update() {}
void ResetMode::Reset() {}
