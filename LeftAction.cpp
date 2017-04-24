#include "LeftAction.h"
#include "actionParms.h"
#include "utility.h"

#include <cmath>

const std::string LeftAction::actionName = "LEFT";

const int LeftAction::triggerOffset = 10;
const int LeftAction::endOffset = 10;
const int LeftAction::abortOffset = 0;

LeftAction::LeftAction()
{}

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
  
  if (monitorStates->length >= triggerOffset)
    {
      
      double radius;

      bool dummy = false;
      for (int i = 0; i < triggerOffset; i++)
	{
	  dummy = dummy || (*monitorStates)[i].dummy;
	}

      
      const Sensing currentState = (*monitorStates)[0];
      const Sensing lastState = (*monitorStates)[1];
      const Sensing previousState = (*monitorStates)[2];

      if (dummy)
	return false;
      
      /* set current and target lane */
      currentLane = floor((*monitorStates)[triggerOffset-1].y);
      targetLane = currentLane + 1;

      if (currentLane == MAX_LANE)
	return false;
      
      /*      double centerX, centerY;*/
      double deltaY, mean, sigma;
      try
	{
	  /*	  circleFit(monitorStates, 6, centerX, centerY, radius);
		  std::cout << "Radius = " << radius << std::endl;*/
	  deltaY = ((*monitorStates)[0].y - (*monitorStates)[triggerOffset-1].y)/triggerOffset;
	  calculateMeanSigma(monitorStates, triggerOffset, mean, sigma);
	}
      catch (int& n){
	return false;}
	  
      if (/*radius <  R_MAX_TURN && radius > R_MIN_TURN*/ deltaY > MAX_SPEED/3 && fabs((*monitorStates)[triggerOffset-1].y - (currentLane + DELTA_Y)) < Y_TOLERANCE + 3*SIGMA_Y)
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
  bool dummy = false;

  for (int i = 0; i < endOffset; i++)
    dummy = dummy || (*monitorStates)[i].dummy;

  if (dummy)
    return false;

  
  /* calculate curvature radius using last three points*/
  double radius;
  
  const Sensing currentState = (*monitorStates)[0];
  const Sensing lastState = (*monitorStates)[1];
  const Sensing previousState = (*monitorStates)[2];
  
  
  double centerX, centerY;
  double mean, sigma;
  try
    {
      /*	  circleFit(monitorStates, 6,
		  centerX, centerY, radius);*/
      calculateMeanSigma(monitorStates, endOffset, mean, sigma);
      //	  std::cout << "End condition mean = " << mean << "\tsigma = " << sigma << std::endl;
    }
  catch (int& n){
    return false;}
  
  /*if (fabs(radius) > R_END_TURN && fabs(currentState.y - (targetLane + 0.5)) < Y_TOLERANCE)*/
  if (fabs(mean - (targetLane + DELTA_Y)) < (Y_TOLERANCE + 3*SIGMA_Y/sqrt(triggerOffset)) && sigma < SIGMA_Y)
    {
      //	  std::cout << "TargetLane = " << targetLane << std::endl;
      return true;
    }
  
  
  return false;
}

bool LeftAction::abortCondition()
{
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
