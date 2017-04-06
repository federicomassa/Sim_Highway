/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the left lane change action
 */

#ifndef RIGHT_ACTION_H
#define RIGHT_ACTION_H

#include "Action.h"
#include "List.h"
#include "State.h"

#include <string>

class RightAction : public Action
{
  double currentLane, targetLane;
 public:
  RightAction();
 RightAction(const Action& a) : Action(a) {currentLane = targetLane = -1;}
  ~RightAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initRuleCategories();
  
  std::string name() const {return actionName;}
  static const std::string actionName;

};


#endif
