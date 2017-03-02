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
  TravelAction(const List<State>&);
  ~TravelAction();
  bool triggerCondition();
  bool endCondition();
  bool abortCondition();

  std::string name() {return "TRAVEL";}
};


#endif
