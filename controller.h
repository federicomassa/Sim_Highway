
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cmath>

#include "EndLine.h"
#include "Logger.h"
#include "State.h"
#include "systemTypes.h"
#include "systemParms.h"

extern Logger LOG;

class Failure;
enum ControlFailure : int;

Control computeControl(Maneuver, const State&, /* For platoon */ List<State>, const Failure::ControlFailure& fail, bool& tooClose, int idx = -1, bool debug = false);
Control computeControl(Maneuver, const State&, /* For platoon */ List<State>, int idx = -1, bool debug = false);

#endif
