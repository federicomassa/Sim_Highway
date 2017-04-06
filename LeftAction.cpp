#include "LeftAction.h"
#include "actionParms.h"
#include "utility.h"

#include <cmath>

const std::string LeftAction::actionName = "LEFT";

LeftAction::LeftAction() {}
LeftAction::~LeftAction() {}

void LeftAction::initRuleCategories()
{
  addRuleCategory("LeftLaneChange");
  addRuleCategory("Safety");
}

bool LeftAction::triggerCondition()
{
  /* LeftAction is triggered if the vehicle turns left with a curvature radius within specific limits */
  
  /* calculate curvature radius using last three points*/

  
  if (monitorStates->length >= 3)
    {
      
      double radius;

      const Sensing currentState = (*monitorStates)[0];
      const Sensing lastState = (*monitorStates)[1];
      const Sensing previousState = (*monitorStates)[2];

      if (currentState.dummy || lastState.dummy || previousState.dummy)
	return false;
      
      /* set current and target lane */
      currentLane = floor(previousState.y);
      targetLane = currentLane + 1;

      if (currentLane == MAX_LANE)
	return false;
      
      double centerX, centerY;
      try
	{
	  circle3points(previousState.x, previousState.y,
			lastState.x, lastState.y,
			currentState.x, currentState.y,
			centerX, centerY, radius);
	}
      catch (int& n){
	return false;}
	  
      if (radius <  R_MAX_TURN && radius > R_MIN_TURN && (currentState.y - (currentLane + DELTA_Y)) < Y_TOLERANCE && currentState.theta > 0)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool LeftAction::endCondition()
{
  /* LeftAction is ended when the vehicle reaches the center of the target lane within
   a specified tolerance */
  
  /* calculate curvature radius using last three points*/
  if (monitorStates->length >= 3)
    { 
      double radius;
      
      const Sensing currentState = (*monitorStates)[0];
      const Sensing lastState = (*monitorStates)[1];
      const Sensing previousState = (*monitorStates)[2];

      if (currentState.dummy || lastState.dummy || previousState.dummy)
	return false;
      
      double centerX, centerY;
      try
	{
	  circle3points(previousState.x, previousState.y,
			lastState.x, lastState.y,
			currentState.x, currentState.y,
			centerX, centerY, radius);
	}
      catch (int& n){
	return false;}
      
      if (fabs(radius) > R_END_TURN && fabs(currentState.y - (targetLane + 0.5)) < Y_TOLERANCE)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool LeftAction::abortCondition()
{
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
