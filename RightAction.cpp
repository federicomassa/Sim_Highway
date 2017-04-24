#include "RightAction.h"
#include "actionParms.h"
#include "utility.h"


#include <cmath>
		   
const std::string RightAction::actionName = "RIGHT";

const int RightAction::triggerOffset = 10;
const int RightAction::endOffset = 10;
const int RightAction::abortOffset = 0;

RightAction::RightAction()
{}

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
  bool dummy = false;

  for (int i = 0; i < triggerOffset; i++)
    dummy = dummy || (*monitorStates)[i].dummy;

  if (dummy)
    return false;
  
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
  
  double deltaY, mean, sigma;
  try
    {
      /*	  circleFit(monitorStates, 6, centerX, centerY, radius);
		  std::cout << "Radius = " << radius << std::endl;*/
      deltaY = ((*monitorStates)[triggerOffset-1].y - (*monitorStates)[0].y)/triggerOffset;
      calculateMeanSigma(monitorStates, triggerOffset, mean, sigma);
    }
  catch (int& n){
    return false;}
  
  if (/*radius <  R_MAX_TURN && radius > R_MIN_TURN*/ deltaY > MAX_SPEED/3 && fabs((*monitorStates)[triggerOffset-1].y - (currentLane + DELTA_Y)) < Y_TOLERANCE + 3*SIGMA_Y)
    return true;       
  
  
  return false;
}

bool RightAction::endCondition()
{
  /* RightAction is ended when the vehicle reaches the center of the target lane within
   a specified toleranc*/
  bool dummy = false;

  for (int i = 0; i < endOffset; i++)
    dummy = dummy || (*monitorStates)[i].dummy;

  if (dummy)
    return false;
  
  /* calculate curvature radius using last three points*/

  /* arbitrarily large number to represent infinity */
  
  double radius;
  
  const Sensing currentState = (*monitorStates)[0];
  const Sensing lastState = (*monitorStates)[1];
  const Sensing previousState = (*monitorStates)[2];      
  
  if (currentState.dummy || lastState.dummy || previousState.dummy)
    return false;

  double centerX, centerY;
  double mean, sigma;
  try
    {
      /*	  circleFit(monitorStates, 6,
		  centerX, centerY, radius);*/
      calculateMeanSigma(monitorStates, 6, mean, sigma);
      //	  std::cout << "End condition mean = " << mean << "\tsigma = " << sigma << std::endl;
    }
  catch (int& n){
    return false;}
  
  /*if (fabs(radius) > R_END_TURN && fabs(currentState.y - (targetLane + 0.5)) < Y_TOLERANCE)*/
  if (fabs(mean - (targetLane + DELTA_Y)) < Y_TOLERANCE + 3*SIGMA_Y/sqrt(endOffset) && sigma < SIGMA_Y)
    {
      //	  std::cout << "TargetLane = " << targetLane << std::endl;
      return true;
    }
  
  
  
  

  return false;
}

bool RightAction::abortCondition()
{
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
