#ifndef FAST_UNICYCLE_H
#define FAST_UNICYCLE_H

#include <cmath>

#include "EndLine.h"
#include "Logger.h"
#include "State.h"
#include "systemTypes.h"
#include "systemParms.h"

extern Logger LOG;

namespace fastUnicycle {
  
  Control computeControl(const Maneuver&, const State&, const Parms&, const List<Sensing>&, int idx = -1, bool debug = false);
}

#endif
