/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the travel action
 */

#ifndef LEFT_OVERTAKE_ACTION_H
#define LEFT_OVERTAKE_ACTION_H

#include "Action.h"
#include "List.h"
#include "State.h"

#include <string>

class LeftOvertakeAction : public Action
{
 private:
  int targetID;
 public:
  LeftOvertakeAction();
 LeftOvertakeAction(const Action& a) : Action(a) {}
  ~LeftOvertakeAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initRuleCategories();
  std::string name() const {return actionName;}
  static const std::string actionName;
};


#endif
