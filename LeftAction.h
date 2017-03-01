/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes the left lane change action
 */

#ifndef LEFT_ACTION_H
#define LEFT_ACTION_H

#include "Action.h"
#include <string>

class LeftAction : public Action
{
 public:
  LeftAction();
  ~LeftAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  std::string name() {return "LEFT";}
};


#endif
