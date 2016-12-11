
#include "controller.h"

#include <iostream>

Control computeControl(Maneuver sigma, const State& q, /* For platoon */ List<State> qList)
{
    Control c;
    
    if(CONF.debug)
    {
        LOG.s << "Computing control for q = " << q << ", sigma = " << sigma << ":";
        LOG.s << EndLine(EndLine::INC);
    }
    
    switch(sigma)
      {
      case PLATOON:
	{
	  /* Platoon case FIXME */
	  // Look for vehicle ahead and behind
	  Iterator<State> is(qList);
	  State tmpQ(0,0,0,0);
	  State aheadQ(0,0,0,0);
	  State behindQ(0,0,0,0);
	  
	  bool isVehicleAhead = false;
	  bool isVehicleBehind = false;
	  
	  for (int i = 0; i < qList.count(); i++) {
	    is(tmpQ);
	    
	    /* If same lane */
	    if (tmpQ.y < floor(q.y) + 1 && tmpQ.y > floor(q.y)) {
	      /* If ahead of you */
	      if (tmpQ.x - q.x > 0 && fabs(tmpQ.desiredV - q.desiredV) < V_TOLERANCE*q.desiredV){
		aheadQ = tmpQ;
		isVehicleAhead = true;
		continue;
	      }
	      /* If behind you */
	      else if (tmpQ.x - q.x < 0 && fabs(tmpQ.desiredV - q.desiredV) < V_TOLERANCE*q.desiredV) {
		behindQ = tmpQ;
		isVehicleBehind = true;
		continue;
	      }
	      
	    }
	    
	    
	    
	  }
	  
	  // if there is both a vehicle ahead and behind
	  if (isVehicleAhead && isVehicleBehind) {
	    c.a = -B_BACK*(q.x - behindQ.x) -B_FORWARD*(q.x - aheadQ.x) - GAMMA*(B_BACK*(q.v - behindQ.v) + B_FORWARD*(q.v - aheadQ.v));
	  }
	  
	  // Platoon rear
	  else if (isVehicleAhead && !isVehicleBehind) {
	    c.a = -B_FORWARD*(q.x - aheadQ.x + D_REF) - GAMMA*B_FORWARD*(q.v - aheadQ.v);
	  }
	  
	  // Platoon leader
	  else if (!isVehicleAhead && isVehicleBehind) {
	    c.a = -B_BACK*(q.x - behindQ.x - D_REF) - GAMMA*B_BACK*(q.v - behindQ.v) - K_LEADER*(q.v - q.desiredV);
	  }
	  
	  else
	    c.a = ACCELERATION;

	  if (fabs(c.a) > ACCELERATION)
	    c.a = fabs(c.a)/c.a*ACCELERATION;
	  
	  if (q.theta != 0){
	    c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
	      / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
	  } else
	    c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
	  
	  break;
	}
      case FAST:
	{
	  
	  if (q.v < q.desiredV)
	    c.a = ACCELERATION;
	  else
	    c.a = 0;
	  
	  
	  if (q.theta != 0){
	    c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
	      / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
	  } else
	    c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
	  
	  break;
	}
      case SLOW:
	c.a = -ACCELERATION;
	if (q.theta != 0){
	  c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
	    / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
	}else{
	  c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
	}
	break;
      case LEFT:
	c.a = ACCELERATION;
	c.omega = MAX_ANGULAR_SPEED;
	break;
      case RIGHT:
	c.a = ACCELERATION;
	c.omega = -MAX_ANGULAR_SPEED;
	break;
      default:
	error("computeControl", "maneuver UNKNOWN passed");
      }
    
    if(c.omega > 0)
    {
        if(c.omega > MAX_ANGULAR_SPEED)
            c.omega = MAX_ANGULAR_SPEED;
        if(q.theta + c.omega > MAX_THETA)
            //c.omega = 0;
            c.omega = MAX_THETA - q.theta;
    }
    else if(c.omega < 0)
    {
        if(c.omega < -MAX_ANGULAR_SPEED)
            c.omega = -MAX_ANGULAR_SPEED;
        if(q.theta + c.omega < -MAX_THETA)
            //c.omega = 0;
            c.omega = -MAX_THETA - q.theta;
    }

    if(CONF.debug)
        LOG.s << c << EndLine(EndLine::DEC);

    return c;
}
