
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cmath>

#include "EndLine.h"
#include "Logger.h"
#include "State.h"
#include "systemTypes.h"
#include "systemParms.h"

extern Logger LOG;

Control computeControl(const Maneuver&, const State&, const Parms&, const List<Sensing>&, int idx = -1, bool debug = false);

#endif
