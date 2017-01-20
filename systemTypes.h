#ifndef SYSTEMTYPES_H
#define SYSTEMTYPES_H

#include "Failure.h"

class State;

/* vehicle's automaton internal vars */
typedef double IntVars; /* target lane */

/* reset function for internal vars */
typedef IntVars (*ResetFunction)(const State&);

/* vehicle parameters */
typedef double Parms; /* max speed */

/* vehicle automaton states */
enum Maneuver { FAST, SLOW, LEFT, RIGHT, PLATOON, UNKNOWN };

/* function to pass from std::string to maneuver */
Maneuver strToManeuver(const std::string& str);

/* function to pass from maneuver to std::string */
std::string maneuverToStr(const Maneuver& sigma);

ostream& operator<<(ostream& os, Maneuver m);
string operator+(const string&, Maneuver);
string operator+(Maneuver, const string&);
/* continuous control */
struct Control { double a; double omega; };

ostream& operator<<(ostream& os, Control c);

#endif
