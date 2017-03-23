/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = List of parameters used to define trigger, end, abort conditions in sub-Action classes.
*/

#ifndef ACTION_PARMS_H
#define ACTION_PARMS_H

#include "definition.h"

/* curvature radius boundaries to trigger left and right turn */
#define R_MIN_TURN 0.6
#define R_MAX_TURN 3
#define R_END_TURN 10
#define R_MIN_TRAVEL 6
#define Y_TOLERANCE 0.1
#define OVERTAKE_TRIGGER_DISTANCE VEHICLE_IMG_W/SCALE

#endif
