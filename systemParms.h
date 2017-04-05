#ifndef SYSTEMPARMS_H
#define SYSTEMPARMS_H

#include "Vector.h"

#define N_MANEUVER 5
#define MIN_LANE 0
#define MAX_LANE 1
#define LANE_HEIGHT 1.0
#define DELTA_Y LANE_HEIGHT / 2.0
  
#define PI 3.14159265358979323846

typedef Vector<Vector<double, 2>, 2> Matrix_2x2;

// Controller parameters:
#define MAX_THETA 0.9
#define MAX_ANGULAR_SPEED 0.06
#define LOOP_GAIN_CONSTANT 1.9
#define MAX_SPEED 0.06
//#define MAX_SPEED 0.08
#define ACCELERATION 0.005
#define ERROR_ACC ACCELERATION/2.0
//#define MAX_SPEED 0.06
//#define ACCELERATION 0.005
//#define MAX_SPEED 1.2
//#define ACCELERATION 0.1


// Visible area parameters:
#define VISIBLE_DISTANCE 8.8

// Interaction distance, it should be a
// fraction of VISIBLE_DISTANCE in order
// to avoid micro-overlaps in the areas
#define INTERACTION_DISTANCE 0.9*VISIBLE_DISTANCE

// Platoon controller parameters
// u_i = -SUM(b_ij (x_i - x_j)) - gamma*SUM(b_ij (v_i - v_j))
#define B_BACK 0.005
#define B_FORWARD 0.005
#define GAMMA 10

#define D_REF 2

#define K_LEADER B_BACK*GAMMA/10.0
//#define V_REF 0.5*MAX_SPEED
#define V_TOLERANCE 0.1


#endif
