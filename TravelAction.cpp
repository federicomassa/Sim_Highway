#include "TravelAction.h"
#include "utility.h"

#include <cmath>
		   

TravelAction::TravelAction(const List<State>& mState) : Action(mState) {}
TravelAction::~TravelAction() {}

bool TravelAction::triggerCondition()
{
  /* TravelAction is triggered if the vehicle travels straight within a certain tolerance in the curvature radius */
  
  /* calculate curvature radius using last three points*/

  
  if (monitorStates.count() >= 3)
    {
      /* arbitrarily large number to represent infinity */
      
      double radius;

      const State* currentState;
      const State* lastState;
      const State* previousState;
      
      monitorStates.getElem(currentState, 0);
      monitorStates.getElem(lastState, 1);
      monitorStates.getElem(previousState, 2);
      
      double centerX, centerY;
      circle3points(previousState->x, previousState->y,
		    lastState->x, lastState->y,
		    currentState->x, currentState->y,
		    centerX, centerY, radius);


      if (fabs(radius) > 6 && fabs(currentState->y - (floor(currentState->y) + 0.5)) < 0.1)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool TravelAction::endCondition()
{
  /* TravelAction is ended when the vehicle reaches the center of the target lane within
   a specified toleranc*/
  
  /* calculate curvature radius using last three points*/

  if (monitorStates.count() >= 3)
    {
      /* arbitrarily large number to represent infinity */
      
      double radius;

      const State* currentState;
      const State* lastState;
      const State* previousState;
      
      monitorStates.getElem(currentState, 0);
      monitorStates.getElem(lastState, 1);
      monitorStates.getElem(previousState, 2);
      
      double centerX, centerY;
      circle3points(previousState->x, previousState->y,
		    lastState->x, lastState->y,
		    currentState->x, currentState->y,
		    centerX, centerY, radius);

      if (fabs(radius) < 6 || fabs(currentState->y - (floor(currentState->y) + 0.5)) > 0.1)
	return true; 
      
    }
  else
    return false;

  return false;
}

bool TravelAction::abortCondition()
{
  
  
  return false;
}
