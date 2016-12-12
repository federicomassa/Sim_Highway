
#ifndef RULES_H
#define RULES_H

#include <cmath>

#include "Area.h"
#include "Transition.h"
#include "systemParms.h"
#include "systemTypes.h"
#include "Vector.h"

/// Rules parameters:
#define N_SUB_EVENT 32
#define N_EVENT 33
/* LANES: see also definition.h */
#define MIN_LANE 0
#define MAX_LANE 1
#define LANE_HEIGHT 1.0
#define DELTA_Y LANE_HEIGHT / 2.0

// Automaton first state:
//#define SIGMA_0 FAST

// Misc:
#define PI 3.14159265358979323846

typedef Vector<Vector<double, 2>, 2> Matrix_2x2;

/// ---------------------------------------------------------------------------
/// Rules parameters
/// ---------------------------------------------------------------------------

double dForward();
double dBack();

/// ---------------------------------------------------------------------------
/// SubEvents functions
/// ---------------------------------------------------------------------------

bool forwardBlocking(const State&, const IntVars&, const State&);
bool leftBlocking(const State&, const IntVars&, const State&);
bool rightBlocking(const State&, const IntVars&, const State&);
bool rightBackBlocking(const State&, const IntVars&, const State&);
bool forwardPresent(const State&, const IntVars&, const State&);
bool backPresent(const State&, const IntVars&, const State&);
bool forwardCompatible(const State&, const IntVars&, const State&);
bool rightForwardCompatible(const State&, const IntVars&, const State&);
bool backCompatible(const State&, const IntVars&, const State&);
bool LeftPlatoonCompatible(const State&, const IntVars&, const State&);
bool RightPlatoonCompatible(const State&, const IntVars&, const State&);
bool RightBlockingOvertakeable(const State&, const IntVars&, const State&);
bool maxLane(const State&, const IntVars&, const State&);
bool minLane(const State&, const IntVars&, const State&);
bool targetLane(const State&, const IntVars&, const State&);
bool linedUp(const State&, const IntVars&, const State&);

/// ---------------------------------------------------------------------------
/// ``Indicator'' functions
/// ---------------------------------------------------------------------------

void forwardArea(const State&, Area&);
void leftArea(const State&, Area&);
void rightArea(const State&, Area&);

/// ---------------------------------------------------------------------------
/// IntVar reset functions
/// ---------------------------------------------------------------------------

IntVars setTargetLane(const State&);
IntVars incTargetLane(const State&);
IntVars decTargetLane(const State&);

/// ---------------------------------------------------------------------------
/// Active area
/// ---------------------------------------------------------------------------

/* return the active area for an agent */
void activeArea(const State&, Area&);

/// ---------------------------------------------------------------------------
/// Rules init
/// ---------------------------------------------------------------------------

void initRules(Vector<SubEvent, N_SUB_EVENT>& se, Vector<Event, N_EVENT>& e,
               Vector<Vector<Transition, N_MANEUVER>, N_MANEUVER>& t,
               Vector<ResetFunction, N_MANEUVER>& r);

#endif
