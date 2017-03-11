/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the travel action
 */

#ifndef TRAVEL_ACTION_H
#define TRAVEL_ACTION_H

#include "Action.h"
#include "List.h"
#include "State.h"

#include <string>

class TravelAction : public Action
{

 public:
  TravelAction();
 TravelAction(const Action& a) : Action(a) {}
  ~TravelAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initBehaviours();
  std::string name() const {return actionName;}
  static const std::string actionName;
};


#endif
