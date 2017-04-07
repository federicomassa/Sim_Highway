#include "TravelAction.h"
#include "utility.h"
#include "actionParms.h"

#include <cmath>

const std::string TravelAction::actionName = "TRAVEL";

const int TravelAction::triggerOffset = 6;
const int TravelAction::endOffset = 6;
const int TravelAction::abortOffset = 0;

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
  bool dummy = false;
  /* calculate curvature radius using last three points*/
  for (int i = 0; i < triggerOffset; i++)
    dummy = dummy || (*monitorStates)[i].dummy;

  if (dummy)
    return false;
  
  double radius;
  
  const Sensing currentState = (*monitorStates)[0];
  const Sensing lastState = (*monitorStates)[1];
  const Sensing previousState = (*monitorStates)[2];
  
  if (currentState.dummy || lastState.dummy || previousState.dummy)
    return false;
  
  double centerX, centerY, mean, sigma;
  try {
    /*	circleFit(monitorStates, 9,
	centerX, centerY, radius);
	std::cout << "Travel radius = " << radius << std::endl;
	std::cout << "Fit distance = " << fitDistance(monitorStates, 9, centerX, centerY, radius) << std::endl;*/
    calculateMeanSigma(monitorStates, triggerOffset, mean, sigma);
  }
  catch(int& n){
    return false;}
  
  double averageY = (currentState.y + lastState.y + previousState.y)/3.0;
  
  /*      if (fabs(radius) > R_MIN_TRAVEL && fabs(averageY - (floor(currentState.y) + 0.5)) < Y_TOLERANCE)*/
  if (fabs(mean - (floor((*monitorStates)[triggerOffset-1].y) + DELTA_Y)) < 3*SIGMA_Y && sigma < 3*SIGMA_Y)
    return true; 

  return false;
}

bool TravelAction::endCondition()
{
  /* TravelAction is ended when the curvature radius is too small or if the vehicle 
   is too distant from the center of the lane */

  bool dummy = false;
  for (int i = 0; i < endOffset;  i++)
    dummy = dummy || (*monitorStates)[i].dummy;

  if (dummy)
    return false;
  
  /* calculate curvature radius using last three points*/     
  double radius;
  
  const Sensing currentState = (*monitorStates)[0];
  const Sensing lastState = (*monitorStates)[1];
  const Sensing previousState = (*monitorStates)[2];
  
  if (currentState.dummy || lastState.dummy || previousState.dummy)
    return false;
  
  
  double centerX, centerY, deltaY, mean, sigma;
  /* circleFit(monitorStates, 6,
     centerX, centerY, radius);*/
  
  //      std::cout << "TravelAction::endCondition, radius = " << radius << std::endl;
  deltaY = ((*monitorStates)[0].y - (*monitorStates)[endOffset-1].y)/endOffset;    
  
  /*  if (fabs(radius) < R_MIN_TRAVEL || fabs(averageY - (floor(currentState.y) + 0.5)) > Y_TOLERANCE)*/
  calculateMeanSigma(monitorStates, endOffset, mean, sigma);

  
  /*      if (fabs(radius) > R_MIN_TRAVEL && fabs(averageY - (floor(currentState.y) + 0.5)) < Y_TOLERANCE)*/
  if (fabs(mean - (floor((*monitorStates)[endOffset-1].y) + DELTA_Y)) > 3*SIGMA_Y || sigma > 3*SIGMA_Y)
    return true; 
  
  

  return false;
}

bool TravelAction::abortCondition()
{
      
  if ((*monitorStates)[1].dummy)
    return true;
  
  return false;
}
