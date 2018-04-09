#include <thread>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#include "Util/generic_util.hpp"

void sleep_for(int millis) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}