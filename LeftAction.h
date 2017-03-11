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
 public:
  LeftAction();
 LeftAction(const Action& a) : Action(a) {}
  ~LeftAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initBehaviours();
  std::string name() const {return actionName;}
  static const std::string actionName;
};


#endif
