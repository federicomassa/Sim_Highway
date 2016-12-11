
#ifndef SYSTEMTYPES_H
#define SYSTEMTYPES_H

#include "State.h"
#include "Failure.h"

/* vehicle's automaton internal vars */
typedef double IntVars; /* target lane */

/* reset function for internal vars */
typedef IntVars (*ResetFunction)(const State&);

/* vehicle parameters */
typedef double Parms; /* max speed */

/* vehicle automaton states */
enum Maneuver { FAST, SLOW, LEFT, RIGHT, PLATOON, UNKNOWN };

ostream& operator<<(ostream& os, Maneuver m);

/* continuous control */
struct Control { double a; double omega; };

ostream& operator<<(ostream& os, Control c);

#endif
