#include "includes.h"

int GetAnglesDiff(double a1, double a2) {
  //https://stackoverflow.com/a/36001014
  return 180 - (180 - (int)a1 + (int)a2) % 360;
}
