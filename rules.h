
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
#define N_EVENT 23
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

bool forwardBlocking(const Sensing&, const IntVars&, const Sensing&);
bool leftBlocking(const Sensing&, const IntVars&, const Sensing&);
bool rightBlocking(const Sensing&, const IntVars&, const Sensing&);
bool rightBackBlocking(const Sensing&, const IntVars&, const Sensing&);
bool forwardPresent(const Sensing&, const IntVars&, const Sensing&);
bool backPresent(const Sensing&, const IntVars&, const Sensing&);
bool forwardCompatible(const Sensing&, const IntVars&, const Sensing&);
bool rightForwardCompatible(const Sensing&, const IntVars&, const Sensing&);
bool backCompatible(const Sensing&, const IntVars&, const Sensing&);
bool LeftPlatoonCompatible(const Sensing&, const IntVars&, const Sensing&);
bool RightPlatoonCompatible(const Sensing&, const IntVars&, const Sensing&);
bool RightBlockingOvertakeable(const Sensing&, const IntVars&, const Sensing&);
bool maxLane(const Sensing&, const IntVars&, const Sensing&);
bool minLane(const Sensing&, const IntVars&, const Sensing&);
bool targetLane(const Sensing&, const IntVars&, const Sensing&);
bool linedUp(const Sensing&, const IntVars&, const Sensing&);

/// ---------------------------------------------------------------------------
/// ``Indicator'' functions
/// ---------------------------------------------------------------------------

void forwardArea(const Sensing&, Area&);
void leftArea(const Sensing&, Area&);
void rightArea(const Sensing&, Area&);

/// ---------------------------------------------------------------------------
/// IntVar reset functions
/// ---------------------------------------------------------------------------

IntVars setTargetLane(const Sensing&);
IntVars incTargetLane(const Sensing&);
IntVars decTargetLane(const Sensing&);

/// ---------------------------------------------------------------------------
/// Active area
/// ---------------------------------------------------------------------------

/* return the active area for an agent */
void activeArea(const Sensing&, Area&);

/// ---------------------------------------------------------------------------
/// Rules init
/// ---------------------------------------------------------------------------

void initRules(Vector<SubEvent, N_SUB_EVENT>& se, Vector<Event, N_EVENT>& e,
               Vector<Vector<Transition, N_MANEUVER>, N_MANEUVER>& t,
               Vector<ResetFunction, N_MANEUVER>& r);

#endif
