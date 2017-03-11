#include "RightAction.h"
#include "actionParms.h"
#include "utility.h"


#include <cmath>
		   
const std::string RightAction::actionName = "RIGHT";

RightAction::RightAction() {}
RightAction::~RightAction() {}

void RightAction::initBehaviours()
{
  addBehaviour("RightLaneChange");
}

bool RightAction::triggerCondition()
{
  /* RightAction is triggered if the vehicle turns right with a curvature radius within specific limits */
  
  /* calculate curvature radius using last three points*/

  
  if (monitorStates->length >= 3)
    {
      /* arbitrarily large number to represent infinity */
      
      double radius;

      const State currentState = (*monitorStates)[0];
      const State lastState = (*monitorStates)[1];
      const State previousState = (*monitorStates)[2];
      
      
      double centerX, centerY;
      circle3points(previousState.x, previousState.y,
		    lastState.x, lastState.y,
		    currentState.x, currentState.y,
		    centerX, centerY, radius);

      if (radius > -R_MAX_TURN && radius < -R_MIN_TURN && (currentState.y - 1.5) < 0.1 && currentState.theta < 0)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool RightAction::endCondition()
{
  /* RightAction is ended when the vehicle reaches the center of the target lane within
   a specified toleranc*/
  
  /* calculate curvature radius using last three points*/

  if (monitorStates->length >= 3)
    {
      /* arbitrarily large number to represent infinity */
      
      double radius;

      const State currentState = (*monitorStates)[0];
      const State lastState = (*monitorStates)[1];
      const State previousState = (*monitorStates)[2];      
      
      double centerX, centerY;
      circle3points(previousState.x, previousState.y,
		    lastState.x, lastState.y,
		    currentState.x, currentState.y,
		    centerX, centerY, radius);

      if (fabs(radius) > 10 && fabs(currentState.y - 0.5) < 0.1)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool RightAction::abortCondition()
{
  
  
  return false;
}
