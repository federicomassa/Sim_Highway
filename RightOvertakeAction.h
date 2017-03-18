/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the travel action
 */

#ifndef RIGHT_OVERTAKE_ACTION_H
#define RIGHT_OVERTAKE_ACTION_H

#include "Action.h"
#include "List.h"
#include "State.h"

#include <string>

class RightOvertakeAction : public Action
{
 private:
  int targetID;
 public:
  RightOvertakeAction();
 RightOvertakeAction(const Action& a) : Action(a) {}
  ~RightOvertakeAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};


#endif
