
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cmath>

#include "EndLine.h"
#include "Logger.h"
#include "State.h"
#include "systemTypes.h"
#include "systemParms.h"

extern Logger LOG;

Control computeControl(Maneuver, const State&, /* For platoon */ List<State>, int idx = -1, bool debug = false);

#endif
