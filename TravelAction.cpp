#include "TravelAction.h"
#include "utility.h"
#include "actionParms.h"

#include <cmath>

const std::string TravelAction::actionName = "TRAVEL";

TravelAction::TravelAction() {}
TravelAction::~TravelAction() {}

void TravelAction::initRuleCategories()
{
  addRuleCategory("Cruise");
  addRuleCategory("Safety");
}

bool TravelAction::triggerCondition()
{
  /* TravelAction is triggered if the vehicle travels straight within a certain tolerance in the curvature radius */
  
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
      try {
	circle3points(previousState.x, previousState.y,
		      lastState.x, lastState.y,
		      currentState.x, currentState.y,
		      centerX, centerY, radius);
      }
      catch(int& n){
	return false;}

      if (fabs(radius) > R_MIN_TRAVEL && fabs(currentState.y - (floor(currentState.y) + 0.5)) < 0.1)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool TravelAction::endCondition()
{
  /* TravelAction is ended when the curvature radius is too small or if the vehicle 
   is too distant from the center of the lane */
  
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
      circle3points(previousState.x, previousState.y,
		    lastState.x, lastState.y,
		    currentState.x, currentState.y,
		    centerX, centerY, radius);

      //      std::cout << "TravelAction::endCondition, radius = " << radius << std::endl;
      
      if (fabs(radius) < R_MIN_TRAVEL || fabs(currentState.y - (floor(currentState.y) + 0.5)) > 0.1)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool TravelAction::abortCondition()
{
      
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
