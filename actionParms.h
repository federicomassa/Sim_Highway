/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = List of parameters used to define trigger, end, abort conditions in sub-Action classes.
*/

#ifndef ACTION_PARMS_H
#define ACTION_PARMS_H

#include "definition.h"

/* curvature radius boundaries to trigger left and right turn */
#define R_MIN_TURN 0.5
#define R_MAX_TURN 3
#define OVERTAKE_TRIGGER_DISTANCE VEHICLE_IMG_W/SCALE

#endif
