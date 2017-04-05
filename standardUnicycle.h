#ifndef STANDARD_UNICYCLE_H
#define STANDARD_UNICYCLE_H

#include <cmath>

#include "EndLine.h"
#include "Logger.h"
#include "State.h"
#include "systemTypes.h"
#include "systemParms.h"

extern Logger LOG;

namespace standardUnicycle
{
  Control computeControl(const Maneuver&, const State&, const Parms&, const List<Sensing>&, int idx = -1, bool debug = false);
}

#endif
