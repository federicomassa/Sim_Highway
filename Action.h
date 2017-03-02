/*
  AUTHOR = Federico Massa, Adriano Fagiolini
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef ACTION_H
#define ACTION_H

#include "State.h"
#include "List.h"
#include <string>


extern int now;

enum ActionStatus {TRIGGERED, ENDED, ABORTED, INACTIVE};

class Action
{
 public:
  Action(const List<State>&);
  int triggerTime;
  int endTime;

  ActionStatus status;

  /* reference to ActionManager's monitor state history */
  const List<State>& monitorStates;
  
  /* Trigger condition specifies the conditions that start the action */
  virtual bool triggerCondition() = 0;
  /* End condition specifies the conditions that end the action */
  virtual bool endCondition() = 0;
  /* Abort condition specifies the conditions that stops the listener of this action */
  virtual bool abortCondition() = 0;
  virtual ~Action() {};
  virtual void listen();
  virtual std::string name() = 0;

  /* contains a string describing the details of the recognized action, same as print() */
  std::string info();
  
  void print();
};

std::string toString(const ActionStatus&);

#endif
