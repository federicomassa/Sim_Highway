#include "standardRules.h"
#include <iostream>
// ---------------------------------------------------------------------------
// Rules parameters
// ---------------------------------------------------------------------------
using namespace standardRules;

double standardRules::dForward()
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

const double D_FORWARD = dForward();

double standardRules::dBack()
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

const double D_BACK = dBack();

// ---------------------------------------------------------------------------
// SubEvents functions
// ---------------------------------------------------------------------------

bool standardRules::forwardBlocking(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if((s2.x - s1.x) > 0 && (s2.x - s1.x) < D_FORWARD &&
        s2.y > floor(s1.y) && s2.y < (floor(s1.y) + 1))
            return true;

    return false;
}

bool standardRules::leftBlocking(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < D_FORWARD &&
        s2.y > (floor(s1.y) + 1) && s2.y < (floor(s1.y) + 2 ))
            return true;
    
    if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < 0 &&
        s2.y > floor(s1.y) && s2.y < (floor(s1.y) + 1))
      return /*TEST true*/ false;
    
    return false;
}

bool standardRules::rightBlocking(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < D_FORWARD &&
        s2.y < floor(s1.y) && s2.y > (floor(s1.y) - 1))
            return true;
    
    return false;
}

bool standardRules::rightBackBlocking(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if((s1.x - s2.x) < D_BACK && s1.x > s2.x &&
        s2.y < floor(s1.y) && s2.y > (floor(s1.y) - 1))
            return true;
    
    return false;
}


bool standardRules::forwardPresent(const Sensing& s1, const IntVars& v1, const Sensing& s2) 
{
  if ((s2.x - s1.x) > 0 &&
      (s2.x - s1.x) < INTERACTION_DISTANCE &&
      s2.y < (floor(s1.y) + 1) && s2.y > (floor(s1.y)))
    return true;

  return false;
}

bool standardRules::backPresent(const Sensing& s1, const IntVars& v1, const Sensing& s2) 
{
  if ((s1.x - s2.x) > 0 &&
      (s1.x - s2.x) < INTERACTION_DISTANCE &&
      s2.y < (floor(s1.y)+1) && s2.y > (floor(s1.y)))
    return true;

  return false;
}

bool standardRules::forwardCompatible(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
  if ((s2.x - s1.x) > 0 && (s2.x - s1.x) < INTERACTION_DISTANCE && s2.y < (floor(s1.y) + 1) && s2.y > floor(s1.y))
    if (fabs(s1.desiredV - s2.desiredV) < V_TOLERANCE*s1.desiredV)
      return true;
  
  return false;
}

bool standardRules::rightForwardIncompatible(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
  if ((s2.x - s1.x) > 0 && (s2.x - s1.x) < INTERACTION_DISTANCE && s2.y < floor(s1.y) && s2.y > floor(s1.y - 1))
    if (fabs(s1.desiredV - s2.desiredV) > V_TOLERANCE*s1.desiredV)
      return true;
  
  return false;
}


bool standardRules::backCompatible(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
  if ((s1.x - s2.x) > 0 && (s1.x - s2.x) < INTERACTION_DISTANCE && s2.y < (floor(s1.y) + 1) && s2.y > floor(s1.y))
    if (fabs(s1.desiredV - s2.desiredV) < V_TOLERANCE*s1.desiredV)
      return true;
  
  return false;
}


bool standardRules::maxLane(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if(floor(s1.y) == (double)MAX_LANE)
        return true;
    
    return false;
}

bool standardRules::minLane(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if(floor(s1.y) == (double)MIN_LANE)
        return true;
    
    return false;
}

bool standardRules::targetLane(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if (floor(s1.y) == v1)
        return true;
    
    return false;
}

bool standardRules::linedUp(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
    if (fabs(s1.y - floor(s1.y) - 0.5) < 0.1 && fabs(s1.theta) < 0.1)
        return true;

    return false;
}

// Another platoon is passing on the left (not necessarily next lane), is it compatible with our desired speed?
bool standardRules::LeftPlatoonCompatible(const Sensing& s1, const IntVars& v1, const Sensing& s2) 
{
  if ((s2.x - s1.x) > 0 && (s2.x - s1.x) < INTERACTION_DISTANCE)
    if (fabs(s1.desiredV - s2.desiredV) < V_TOLERANCE*s1.desiredV)
      if (s1.y < floor(s2.y))
	return true;

  return false;
}

// Another platoon is passing on the right (not necessarily next lane), is it more compatible with our desired speed?
bool standardRules::RightPlatoonCompatible(const Sensing& s1, const IntVars& v1, const Sensing& s2) 
{
  if ((s2.x - s1.x) > 0 && (s2.x - s1.x) < INTERACTION_DISTANCE)
    if (fabs(s1.desiredV - s1.v) > fabs(s1.desiredV - s2.v))
      if (s1.y > floor(s2.y + 1.0))
	return true;

  return false;
}

// On the right there is a vehicle with a greater or equal speed -> can't overtake it
bool standardRules::RightBlockingOvertakeable(const Sensing& s1, const IntVars& v1, const Sensing& s2)
{
  // if (s1.x < 2.167 && s1.x > 2.165)
  //   if (s1.y < 1.501 && s1.y > 1.499)
  //     std::cout << "X: " << s2.x << " - Y: " << s2.y << " - THETA: " << s2.theta << " - V: " << s2.v << " - myDesired: " << s1.desiredV << std::endl;


  if((s1.x - s2.x) < D_BACK && (s2.x - s1.x) < D_FORWARD &&
     s2.y < floor(s1.y) && s2.y > (floor(s1.y) - 1))
    if (s1.desiredV > s2.desiredV + V_TOLERANCE*s1.desiredV)
      return true;
  
  return false;

}

// ---------------------------------------------------------------------------
// Indicator functions
// ---------------------------------------------------------------------------

void standardRules::forwardArea(const Sensing& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();
    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x;
    bounds[0][1] = s.x + D_FORWARD;
    bounds[1][0] = floor(s.y);
    bounds[1][1] = floor(s.y) + 1;
    ind.addRect(bounds);
}

// CHECK THIS: only useful for monitor?
void standardRules::backArea(const Sensing& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();
    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x - D_BACK;
    bounds[0][1] = s.x;
    bounds[1][0] = floor(s.y);
    bounds[1][1] = floor(s.y) + 1;
    ind.addRect(bounds);
}


void standardRules::leftArea(const Sensing& s, Area& ind)
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
    
    bounds[0][0] = s.x - D_BACK;
    bounds[0][1] = s.x;
    bounds[1][0] = floor(s.y);
    bounds[1][1] = floor(s.y) + 1;
    ind.addRect(bounds);
}

void standardRules::rightArea(const Sensing& s, Area& ind)
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

void standardRules::forwardVisibleArea(const Sensing& s, Area &ind)
{
  if (!ind.isEmpty())
    ind.purge();

  Matrix_2x2 bounds;
  bounds[0][0] = s.x;
  bounds[0][1] = s.x + INTERACTION_DISTANCE;
  bounds[1][0] = floor(s.y);
  bounds[1][1] = floor(s.y) + 1;
  ind.addRect(bounds);
  
}

void standardRules::backVisibleArea(const Sensing& s, Area &ind)
{
  if (!ind.isEmpty())
    ind.purge();

  Matrix_2x2 bounds;
  bounds[0][0] = s.x - INTERACTION_DISTANCE;
  bounds[0][1] = s.x;
  bounds[1][0] = floor(s.y);
  bounds[1][1] = floor(s.y) + 1;
  ind.addRect(bounds);
  
}

void standardRules::rightBackArea(const Sensing& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();

    if (floor(s.y) == MIN_LANE)
      return;
    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x - D_BACK;
    bounds[0][1] = s.x;
    bounds[1][0] = floor(s.y) - 1;
    bounds[1][1] = floor(s.y);
    ind.addRect(bounds);
}

void standardRules::leftForwardVisibleArea(const Sensing& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();

    if (floor(s.y) == MAX_LANE)
      return;

    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x;
    bounds[0][1] = s.x + INTERACTION_DISTANCE;
    bounds[1][0] = floor(s.y) + 1;
    bounds[1][1] = MAX_LANE + 1;
    ind.addRect(bounds);
}

void standardRules::rightForwardVisibleArea(const Sensing& s, Area& ind)
{
    if (!ind.isEmpty())
        ind.purge();

    if (floor(s.y) == MIN_LANE)
      return;

    
    Matrix_2x2 bounds;
    bounds[0][0] = s.x;
    bounds[0][1] = s.x + INTERACTION_DISTANCE;
    bounds[1][0] = MIN_LANE;
    bounds[1][1] = floor(s.y);
    ind.addRect(bounds);
}


// ---------------------------------------------------------------------------
// IntVars reset functions
// ---------------------------------------------------------------------------

IntVars standardRules::setTargetLane(const Sensing& s)
{
    IntVars v = floor(s.y);
    return v;
}

IntVars standardRules::incTargetLane(const Sensing& s)
{
    IntVars v = floor(s.y) + 1;
    return v;
}

IntVars standardRules::decTargetLane(const Sensing& s)
{
    IntVars v = floor(s.y) - 1;
    return v;
}

// ---------------------------------------------------------------------------
// Active area
// ---------------------------------------------------------------------------

/**
 * this function returns the active area for an agent
 */
void standardRules::activeArea(const Sensing& s, Area& active)
{
    if(CONF.debug)
        LOG.s << "Computing active area for state " << s << ":" << EndLine();
    if (!active.isEmpty())
        active.purge();
    double upperBound;
    if (floor(s.y) == MAX_LANE)
        upperBound = MAX_LANE + 1;
    else
        upperBound = MAX_LANE + 2;
    double lowerBound;
    if (floor(s.y) == MIN_LANE)
        lowerBound = MIN_LANE;
    else
        lowerBound = MIN_LANE - 1;
    Matrix_2x2 bounds;
    bounds[0][0] = s.x - VISIBLE_DISTANCE; //It was -D_BACK : +D_FORWARD
    bounds[0][1] = s.x + VISIBLE_DISTANCE;
    bounds[1][0] = lowerBound;
    bounds[1][1] = upperBound;
    active.addRect(bounds);
    if(CONF.debug)
        LOG.s << active << EndLine();
}


// ---------------------------------------------------------------------------
// Rules init
// ---------------------------------------------------------------------------

void standardRules::initStandardRules(DynVector<SubEvent>& se, DynVector<Event>& e,
               Vector<Vector<Transition, N_MANEUVER>, N_MANEUVER>& t,
               Vector<ResetFunction, N_MANEUVER>& r)
{

    int k = 0;

    se.init(N_SUB_EVENT);
    e.init(N_EVENT);
    
    /* SubEvent initialization */
    //0
    se[k].init(
        forwardBlocking, forwardArea,
        OR, "Someone is in front of me", k
    );
    k++;
    //1
    se[k].init(
        forwardBlocking, forwardArea,
        NOR, "Nobody is in front of me", k
    );
    k++;
    //2
    se[k].init(
        leftBlocking, leftArea,
        OR, "Someone is on my left", k
    );
    k++;
    //3
    se[k].init(
        leftBlocking, leftArea,
        NOR, "Nobody is on my left", k
    );
    k++;
    //4
    se[k].init(
        rightBlocking, rightArea,
        OR, "Someone is on my right", k
    );
    k++;
    //5
    se[k].init(
        rightBlocking, rightArea,
        NOR, "Nobody is on my right", k
    );
    k++;
    //6
    se[k].init(
        maxLane, NULL,
        SINGLE, "I'm on the maximum lane", k
    );
    k++;
    //7
    se[k].init(
        maxLane, NULL,
        NSINGLE, "I'm not on the maximum lane", k
    );
    k++;
    //8
    se[k].init(
        minLane, NULL,
        SINGLE, "I'm on the minimum lane", k
    );
    k++;
    //9
    se[k].init(
        minLane, NULL,
        NSINGLE, "I'm not on the minimum lane",k 
    );
    k++;
    //10
    se[k].init(
        targetLane, NULL,
        SINGLE, "I'm on my target lane", k
    );
    k++;
    //11
    se[k].init(
        targetLane, NULL,
        NSINGLE, "I'm not on my target lane", k
    );
    k++;
    //12
    se[k].init(
        linedUp, NULL,
        SINGLE, "I'm lined up and in the center of the lane", k
    );
    k++;
    //13
    se[k].init(
        linedUp, NULL,
        NSINGLE, "I'm not lined up and in the center of the lane", k
    );
    k++;

    //14
    se[k].init(
	       forwardPresent, forwardVisibleArea,
	       OR, "I can see someone in front of me", k);
    k++;

    //15
    se[k].init(
	       forwardPresent, forwardVisibleArea,
	       NOR, "I can't see someone in front of me", k);
    k++;
    
    //16
    se[k].init(
	       backPresent, backVisibleArea,
	       OR, "I can see someone behind me", k);
    k++;

    //17
    se[k].init(
	       backPresent, backVisibleArea,
	       NOR, "I can't see someone behind me", k);
    k++;
    
    //18
    se[k].init(
	       forwardCompatible, forwardVisibleArea,
	       OR, "The vehicle in front of me moves with a velocity compatible with mine", k);
    k++;
    
    //19
    se[k].init(
	       forwardCompatible, forwardVisibleArea,
	       NOR, "There is no vehicle in front of me that moves with a velocity compatible with mine", k);
    k++;

    //20
    se[k].init(
	       backCompatible, backVisibleArea,
	       OR, "The vehicle behind me moves with a velocity compatible with mine", k);
    k++;

    //21
    se[k].init(
	       backCompatible, backVisibleArea,
	       NOR, "There is no vehicle behind me that moves with a velocity compatible with mine", k);
    k++;

    //22
    se[k].init(
	       rightBackBlocking, rightBackArea,
	       OR, "The vehicle on the right lane blocks me from behind", k);
    k++;

    //23
    se[k].init(
	       rightBackBlocking, rightBackArea,
	       NOR, "The vehicle on the right lane does not block me from behind", k);
    k++;

    //24
    se[k].init(
	       rightForwardIncompatible, rightForwardVisibleArea,
	       NOR, "The vehicle on the right lane has a velocity compatible with mine", k);
    k++;

    //25
    se[k].init(
	       rightForwardIncompatible, rightForwardVisibleArea,
	       OR, "The vehicle on the right lane has a velocity incompatible with mine", k);
    k++;
	       
    //26
    se[k].init(LeftPlatoonCompatible, leftForwardVisibleArea,
	       OR, "There is a platoon on a left lane with a velocity more compatible with mine", k);
    k++;

    //27
    se[k].init(LeftPlatoonCompatible, leftForwardVisibleArea,
	       NOR, "There isn't a platoon on a left lane with a velocity more compatible with mine", k);
    k++;

    //28
    se[k].init(RightPlatoonCompatible, rightForwardVisibleArea,
	       OR, "There is a platoon on a right lane with a velocity more compatible with mine", k);
    k++;

    //29
    se[k].init(RightPlatoonCompatible, rightForwardVisibleArea,
	       NOR, "There isn't a platoon on a right lane with a velocity more compatible with mine", k);
    k++;

    //30
    se[k].init(RightBlockingOvertakeable, rightArea,
	       OR, "There is at least one right blocking vehicle that can be overtaken", k);
    k++;

    //31
    se[k].init(RightBlockingOvertakeable, rightArea,
	       NOR, "There isn't any right blocking vehicle that can be overtaken", k);
    k++;

    
    /* error handling */
    if (k != N_SUB_EVENT)
        error("initStandardRules", "N_SUB_EVENT differs from number of SubEvents");



    /* Event definition */

    List<Event*> eList;
    List<SubEvent*> seList;
    k = 0;
    
    /* FAST -> FAST */

    /*    seList.insTail(&se[4]);    
    seList.insTail(&se[14]);
    seList.insTail(&se[19]);
    seList.insTail(&se[21]);
    seList.insTail(&se[30]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[1]);
    seList.insTail(&se[8]);
    seList.insTail(&se[16]);
    seList.insTail(&se[21]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[1]);
    seList.insTail(&se[13]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]); */

    t[FAST][FAST].init(eList);
    eList.purge();

    /* FAST -> SLOW */
    
    seList.insTail(&se[0]);
    seList.insTail(&se[2]);
    seList.insTail(&se[12]);
    seList.insTail(&se[19]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[0]);
    seList.insTail(&se[6]);
    seList.insTail(&se[12]);
    seList.insTail(&se[19]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[0]);
    seList.insTail(&se[13]);
    seList.insTail(&se[19]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[1]);
    seList.insTail(&se[4]);
    seList.insTail(&se[31]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    

    t[FAST][SLOW].init(eList);
    eList.purge();
    
    /* FAST -> LEFT */

    seList.insTail(&se[0]);
    seList.insTail(&se[3]);
    seList.insTail(&se[7]);
    seList.insTail(&se[12]);
    seList.insTail(&se[19]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    t[FAST][LEFT].init(eList);
    eList.purge();

    /* FAST -> RIGHT */

    seList.insTail(&se[1]);
    seList.insTail(&se[5]);
    seList.insTail(&se[9]);
    seList.insTail(&se[12]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    t[FAST][RIGHT].init(eList);
    eList.purge();

    /* SLOW -> FAST */

    seList.insTail(&se[1]);
    seList.insTail(&se[4]);
    seList.insTail(&se[30]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[1]);
    seList.insTail(&se[5]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    t[SLOW][FAST].init(eList);
    eList.purge();

    /* SLOW -> SLOW */
    
    /*seList.insTail(&se[0]);
    seList.insTail(&se[2]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[0]);
    seList.insTail(&se[6]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    seList.insTail(&se[0]);
    seList.insTail(&se[13]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[1]);
    seList.insTail(&se[4]);
    seList.insTail(&se[31]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/

    t[SLOW][SLOW].init(eList);
    eList.purge();

    /* SLOW -> LEFT */

    seList.insTail(&se[0]);
    seList.insTail(&se[3]);
    seList.insTail(&se[7]);
    seList.insTail(&se[12]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    t[SLOW][LEFT].init(eList);
    eList.purge();

    /* SLOW -> RIGHT */
    
    t[SLOW][RIGHT].init(eList);
    eList.purge();

    /* LEFT -> FAST */

    seList.insTail(&se[10]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    t[LEFT][FAST].init(eList);
    eList.purge();

    /* LEFT -> SLOW */
    
    t[LEFT][SLOW].init(eList);
    eList.purge();
    
    /* LEFT -> LEFT */

    /*    seList.insTail(&se[11]);
    //    seList.insTail(&se[3]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/
    
    t[LEFT][LEFT].init(eList);
    eList.purge();
    
    /* LEFT -> RIGHT */
    
    t[SLOW][RIGHT].init(eList);
    eList.purge();
    
    /* RIGHT -> FAST */

    seList.insTail(&se[10]);
    seList.insTail(&se[19]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    /*    seList.insTail(&se[4]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/

    t[RIGHT][FAST].init(eList);
    eList.purge();
    
    /* RIGHT -> SLOW */
    
    t[RIGHT][SLOW].init(eList);
    eList.purge();
    
    /* RIGHT -> LEFT */
    
    t[RIGHT][LEFT].init(eList);
    eList.purge();
    
    /* RIGHT -> RIGHT */

    /*seList.insTail(&se[11]);
    seList.insTail(&se[5]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/
    
    t[RIGHT][RIGHT].init(eList);
    eList.purge();  



    /* PLATOON -> PLATOON */
    
    /*    seList.insTail(&se[14]);
    seList.insTail(&se[18]);    
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[16]);
    seList.insTail(&se[20]);
    seList.insTail(&se[1]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[14]);
    seList.insTail(&se[19]);
    seList.insTail(&se[6]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/

    t[PLATOON][PLATOON].init(eList);
    eList.purge();

    /* PLATOON -> FAST */
    
    seList.insTail(&se[15]);
    seList.insTail(&se[17]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[14]);
    seList.insTail(&se[19]);
    //seList.insTail(&se[16]);
    seList.insTail(&se[21]);    
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[15]);
    //seList.insTail(&se[16]);
    seList.insTail(&se[21]);    
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[17]);
    seList.insTail(&se[14]);
    seList.insTail(&se[19]);    
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    
    seList.insTail(&se[5]);
    seList.insTail(&se[9]);
    seList.insTail(&se[15]);
    seList.insTail(&se[16]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    


    t[PLATOON][FAST].init(eList);
    eList.purge();


    
    /* PLATOON -> SLOW */

    seList.insTail(&se[1]);
    seList.insTail(&se[4]);
    seList.insTail(&se[9]);
    seList.insTail(&se[31]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    t[PLATOON][SLOW].init(eList);
    eList.purge();

    /* PLATOON -> LEFT */

    seList.insTail(&se[19]);
    seList.insTail(&se[7]);
    seList.insTail(&se[0]);
    seList.insTail(&se[3]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    /*seList.insTail(&se[3]);
    seList.insTail(&se[7]);
    seList.insTail(&se[12]);
    seList.insTail(&se[26]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/


    t[PLATOON][LEFT].init(eList);
    eList.purge();



    /* PLATOON -> RIGHT */

    /*    seList.insTail(&se[23]);
    seList.insTail(&se[24]);
    seList.insTail(&se[9]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);*/
    

    t[PLATOON][RIGHT].init(eList);
    eList.purge();

    /* FAST -> PLATOON */

    seList.insTail(&se[14]);
    seList.insTail(&se[18]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[16]);
    seList.insTail(&se[20]);
    seList.insTail(&se[4]);
    seList.insTail(&se[9]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[16]);
    seList.insTail(&se[20]);
    seList.insTail(&se[8]);
    seList.insTail(&se[0]);
    seList.insTail(&se[19]);
    seList.insTail(&se[2]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);

    seList.insTail(&se[16]);
    seList.insTail(&se[20]);
    seList.insTail(&se[8]);
    seList.insTail(&se[0]);
    seList.insTail(&se[19]);
    seList.insTail(&se[6]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);
    
    
    t[FAST][PLATOON].init(eList);
    eList.purge();

    /* SLOW -> PLATOON */

    t[SLOW][PLATOON].init(eList);
    eList.purge();

    /* LEFT -> PLATOON */

    

    t[LEFT][PLATOON].init(eList);
    eList.purge();

    /* RIGHT -> PLATOON */

    seList.insTail(&se[10]);
    seList.insTail(&se[18]);
    e[k].init(seList, k);
    seList.purge();
    eList.insTail(&e[k++]);


    t[RIGHT][PLATOON].init(eList);
    eList.purge();



    /* IntVar reset functions */ 
    r[FAST] = setTargetLane;
    r[SLOW] = setTargetLane;
    r[PLATOON] = setTargetLane;
    r[LEFT] = incTargetLane;
    r[RIGHT] = decTargetLane;
    
    /* error handling */
    if (k != N_EVENT)
        error("initStandardRules", "N_EVENTS differs from number of Events");
}