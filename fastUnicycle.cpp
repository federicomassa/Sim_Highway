#include "fastUnicycle.h"
#include "rules.h"
#include <iostream>

const double angular_speed = MAX_ANGULAR_SPEED*1.5;

Control fastUnicycle::computeControl(const Maneuver& sigma, const State& q, const Parms& p, const List<Sensing>& sList, int idx, bool debug)
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
	  if (debug)
	    {
	      std::cout << "See: " << sList.count() << " vehicles" << std::endl;
	      const Sensing* s;
	      for (int i = 0; i < sList.count(); i++)
		{
		  sList.getElem(s, i);
		  std::cout << *s << std::endl;
		}
	    }
	  
	  /* Platoon case FIXME */
	  // Look for vehicle ahead and behind
	  Iterator<Sensing> is(sList);
	  Sensing tmpQ;
	  Sensing aheadQ;
	  Sensing behindQ;
	  
	  bool isVehicleAheadCompatible = false;
	  bool isVehicleBehindCompatible = false;
	  bool isVehicleAheadBlocking = false;


	  /* dummy sensing to use rule functions */
	  Sensing myS(idx, q, p);
	  
	  while(is(tmpQ))
	    {
	    
	      /* If same lane */
	    if (tmpQ.y < floor(q.y) + 1 && tmpQ.y > floor(q.y)) {
	      /* If ahead of you */
	      if (tmpQ.x - q.x > 0 && fabs(tmpQ.desiredV - p.desiredV) < V_TOLERANCE*p.desiredV){
		aheadQ = tmpQ;
		isVehicleAheadCompatible = true;
		continue;
	      }
	      /* If behind you */
	      else if (tmpQ.x - q.x < 0 && fabs(tmpQ.desiredV - p.desiredV) < V_TOLERANCE*p.desiredV) {
		behindQ = tmpQ;
		isVehicleBehindCompatible = true;
		continue;
	      }

	      else if (standardRules::forwardBlocking(myS, floor(q.y), tmpQ)) {
		aheadQ = tmpQ;
		isVehicleAheadBlocking = true;
		continue;
	      }
	    
	      
	    
	    }
	  }

	  
	  // if there is both a vehicle ahead and behind
	  if (isVehicleAheadCompatible && isVehicleBehindCompatible) {
	    c.a = -B_BACK*(q.x - behindQ.x) -B_FORWARD*(q.x - aheadQ.x) - GAMMA*(B_BACK*(q.v - behindQ.v) + B_FORWARD*(q.v - aheadQ.v));
	  }
	  
	  // Platoon rear
	  else if (isVehicleAheadCompatible && !isVehicleBehindCompatible) {
	    c.a = -B_FORWARD*(q.x - aheadQ.x + D_REF) - GAMMA*B_FORWARD*(q.v - aheadQ.v);
	  }
	  
	  // Platoon leader
	  else if (!isVehicleAheadCompatible && isVehicleBehindCompatible) {
	      
	    
	    if (isVehicleAheadBlocking) {
	      c.a = -B_BACK*(q.x - behindQ.x - D_REF) -B_FORWARD*(q.x - aheadQ.x + standardRules::dForward()) - GAMMA*(B_BACK*(q.v - behindQ.v) + B_FORWARD*(q.v - aheadQ.v));
	    }
	    else
	      {
		c.a = -B_BACK*(q.x - behindQ.x - D_REF) /*- GAMMA*B_BACK*(q.v - behindQ.v)*/ - 10*K_LEADER*(q.v - p.desiredV);

	      }

	    
	  }
	  
	  else
	    {
	      c.a = ACCELERATION;
	      if (debug)
		std::cout << "no case" << std::endl;
	    } 

	  /*	  if (fabs(c.a) > ACCELERATION)
		  c.a = fabs(c.a)/c.a*ACCELERATION;*/
	  
	  if (q.theta != 0){
	    c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
	      / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
	  } else
	    c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
			
	  
	  break;
	}
      case FAST:
	{
	  
	  if (q.v < p.desiredV)
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
	if (q.v < p.desiredV)
	  c.a = ACCELERATION;
	else
	  c.a = 0;
	
	c.omega = angular_speed;
	break;
      case RIGHT:
	if (q.v < p.desiredV)
	  c.a = ACCELERATION;
	else
	  c.a = 0;
	
	c.omega = -angular_speed;
	break;
      default:
	error("computeControl", "maneuver UNKNOWN passed");
      }

    
    if(c.omega > 0)
    {
        if(c.omega > angular_speed)
            c.omega = angular_speed;
        if(q.theta + c.omega > MAX_THETA)
            //c.omega = 0;
            c.omega = MAX_THETA - q.theta;
    }
    else if(c.omega < 0)
    {
        if(c.omega < -angular_speed)
            c.omega = -angular_speed;
        if(q.theta + c.omega < -MAX_THETA)
            //c.omega = 0;
            c.omega = -MAX_THETA - q.theta;
    }

    if(CONF.debug)
        LOG.s << c << EndLine(EndLine::DEC);

    return c;
}
