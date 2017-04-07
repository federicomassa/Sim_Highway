/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the left lane change action
 */

#ifndef LEFT_ACTION_H
#define LEFT_ACTION_H

#include "Action.h"
#include "List.h"
#include "State.h"

#include <string>

class LeftAction : public Action
{
  double currentLane, targetLane;
  static const int triggerOffset;
  static const int endOffset;
  static const int abortOffset;
 public:
  LeftAction();
 LeftAction(const Action& a) : Action(a) {currentLane = targetLane = -1;}
  ~LeftAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  int getTriggerOffset() const {return triggerOffset;}
  int getEndOffset() const {return endOffset;}
  int getAbortOffset() const {return abortOffset;}
  
  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};


#endif
