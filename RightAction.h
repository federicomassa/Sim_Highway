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
 public:
  RightAction(const List<State>&);
  ~RightAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  void initBehaviours();
  
  std::string name() {return "RIGHT";}
};


#endif
