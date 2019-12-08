#include "mode.h"

Mode * Mode::current = nullptr;

void Mode::Set(Mode *m) {
  if (current)
    current->Reset();

  current = m;
  current->Init();
}


