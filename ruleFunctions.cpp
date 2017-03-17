#include "ruleFunctions.h"
#include "systemParms.h"
#include <iostream>
// ---------------------------------------------------------------------------
// Rules parameters
// ---------------------------------------------------------------------------

double ruleFunctions::dForward()
{
  double d;
    /* Breaking space */
    d = 0.5 * pow(MAX_SPEED, 2) / ACCELERATION; 
    /* Steering space */
    if(DELTA_Y > MAX_SPEED ||
        acos((MAX_SPEED - DELTA_Y) / MAX_SPEED) > MAX_THETA)
    {
        d += MAX_SPEED * sin(MAX_THETA);
        d += (DELTA_Y - MAX_SPEED * (1 - cos(MAX_THETA))) / tan(MAX_THETA);
    }
    else
    {
        d += MAX_SPEED * sqrt(1 - pow((MAX_SPEED - DELTA_Y) / MAX_SPEED, 2) );
    }
    d += (double)VEHICLE_IMG_W / (double)SCALE;
    
    return d;
}

const double D_FORWARD = ruleFunctions::dForward();

double ruleFunctions::dBack()
{
    double t = 0;
    double d = 0;
    
    if(DELTA_Y > MAX_SPEED ||
        acos((MAX_SPEED - DELTA_Y) / MAX_SPEED) > MAX_THETA)
    {
        t = MAX_THETA / MAX_ANGULAR_SPEED;
        t += (DELTA_Y - MAX_SPEED * (1 - cos(MAX_THETA))) /
             (MAX_SPEED * sin(MAX_THETA));
        d = MAX_SPEED * sin(MAX_THETA);
        d += (DELTA_Y - MAX_SPEED * (1 - cos(MAX_THETA))) / tan(MAX_THETA);
    }
    else
    {
        t = acos(1 - DELTA_Y / MAX_SPEED) / MAX_ANGULAR_SPEED;
        d = MAX_SPEED * sqrt(1 - pow((MAX_SPEED - DELTA_Y) / MAX_SPEED, 2));
    }
    
    return dForward() + t * MAX_SPEED - d;
}

const double D_BACK = ruleFunctions::dBack();


// ---------------------------------------------------------------------------
// SubEvents functions
// ---------------------------------------------------------------------------

bool ruleFunctions::forwardBlocking(const State& s1, const IntVars& v1, const State& s2)
{
    if((s2.x - s1.x) > 0 && (s2.x - s1.x) < D_FORWARD*2 &&
        s2.y > floor(s1.y) && s2.y < (floor(s1.y) + 1))
            return true;

    return false;
}

bool ruleFunctions::forwardDangerous(const State& s1, const IntVars& v1, const State& s2)
{
    if((s2.x - s1.x) > 0 && (s2.x - s1.x) < D_FORWARD &&
        s2.y > floor(s1.y) && s2.y < (floor(s1.y) + 1))
            return true;

    return false;
}


bool ruleFunctions::leftBlocking(const State& s1, const IntVars& v1, const State& s2)
{
    if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < D_FORWARD &&
        s2.y > (floor(s1.y) + 1) && s2.y < (floor(s1.y) + 2 ))
            return true;
        
    return false;
}

bool ruleFunctions::rightBlocking(const State& s1, const IntVars& v1, const State& s2)
{
    if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < D_FORWARD &&
       s2.y > (floor(s1.y) - 1) && s2.y < floor(s1.y))
            return true;
        
    return false;
}

bool ruleFunctions::minLane(const State& s1, const IntVars& v1, const State& s2)
{
    if(floor(s1.y) == (double)MIN_LANE)
        return true;
    
    return false;
}

bool ruleFunctions::trueFunction(const State& s1, const IntVars& v1, const State& s2)
{
    return true;
}


/* Area functions */

void ruleFunctions::forwardArea(const State& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();
    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x;
    bounds[0][1] = s.x + D_FORWARD*2; //safety distance x 2
    bounds[1][0] = floor(s.y);
    bounds[1][1] = floor(s.y) + 1;
    ind.addRect(bounds);
}

void ruleFunctions::forwardDangerousArea(const State& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();
    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x;
    bounds[0][1] = s.x + D_FORWARD; //safety distance
    bounds[1][0] = floor(s.y);
    bounds[1][1] = floor(s.y) + 1;
    ind.addRect(bounds);
}


void ruleFunctions::leftArea(const State& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();

    if (floor(s.y) == MAX_LANE)
      return;
    
    Matrix_2x2 bounds;
    
    bounds[0][0] = s.x - D_BACK;
    bounds[0][1] = s.x + D_FORWARD;
    bounds[1][0] = floor(s.y) + 1;
    bounds[1][1] = floor(s.y) + 2;
    ind.addRect(bounds);
}

void ruleFunctions::rightArea(const State& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();

    if (floor(s.y) == MIN_LANE)
      return;
    
    Matrix_2x2 bounds;
    
    bounds[0][0] = s.x - D_BACK;
    bounds[0][1] = s.x + D_FORWARD;
    bounds[1][0] = floor(s.y) - 1;
    bounds[1][1] = floor(s.y);
    ind.addRect(bounds);
}
