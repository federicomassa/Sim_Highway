#include "RightAction.h"
#include "actionParms.h"
#include "utility.h"


#include <cmath>
		   
const std::string RightAction::actionName = "RIGHT";

RightAction::RightAction() {}
RightAction::~RightAction() {}

void RightAction::initRuleCategories()
{
  addRuleCategory("RightLaneChange");
  addRuleCategory("Safety");
}

bool RightAction::triggerCondition()
{
  /* RightAction is triggered if the vehicle turns right with a curvature radius within specific limits */
  
  /* calculate curvature radius using last three points*/

  
  if (monitorStates->length >= 3)
    {
      /* arbitrarily large number to represent infinity */
      
      double radius;

      const Sensing currentState = (*monitorStates)[0];
      const Sensing lastState = (*monitorStates)[1];
      const Sensing previousState = (*monitorStates)[2];

      if (currentState.dummy || lastState.dummy || previousState.dummy)
	return false;
      
      currentLane = floor(previousState.y);
      targetLane = floor(previousState.y) - 1;

      if (currentLane == MIN_LANE)
	return false;
      
      double centerX, centerY;
      try {
	circle3points(previousState.x, previousState.y,
		      lastState.x, lastState.y,
		      currentState.x, currentState.y,
		      centerX, centerY, radius);
      }
      catch (int& n){
	return false;}
      
      if (radius > -R_MAX_TURN && radius < -R_MIN_TURN && (currentState.y - (currentLane + 0.5)) < 0.1 && currentState.theta < 0)
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

      const Sensing currentState = (*monitorStates)[0];
      const Sensing lastState = (*monitorStates)[1];
      const Sensing previousState = (*monitorStates)[2];      

      if (currentState.dummy || lastState.dummy || previousState.dummy)
	return false;
      
      
      double centerX, centerY;
      circle3points(previousState.x, previousState.y,
		    lastState.x, lastState.y,
		    currentState.x, currentState.y,
		    centerX, centerY, radius);

      if (fabs(radius) > R_END_TURN && fabs(currentState.y - (targetLane + 0.5)) < Y_TOLERANCE)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool RightAction::abortCondition()
{
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
