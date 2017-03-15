/*
  AUTHOR = Federico Massa, Adriano Fagiolini
  EMAIL = fedemassa91@gmail.com
  DESCRIPTION = This is an abstract class representing a general action of a vehicle, which can be
                a lane change, an overtake, etc. The specific actions inherit from this class.
 */

#ifndef ACTION_H
#define ACTION_H

#include "State.h"
#include "Vector.h"
#include <string>


extern int now;

enum ActionStatus {TRIGGERED, ENDED, ABORTED, INACTIVE};

class Action
{
  /* Each action is registered to a series of behaviours of which they must to obey the rules.
     Ex. both the action "travel" and "overtake" are registered to the behaviour "cruise", of which
     they should respect the rules. */
  List<std::string> behaviourList;
  
 protected:
  void addBehaviour(const std::string& beh) {behaviourList.insHead(beh);}
  /* reference to ActionManager's monitor state history */
  const Vector<State, 10>* monitorStates;
 public:
  Action();
  Action(const Action&);

  void init(const Vector<State, 10>&);

  int triggerTime;
  int endTime;
  ActionStatus status;
  
  /* Trigger condition specifies the conditions that start the action */
  virtual bool triggerCondition() = 0;
  /* End condition specifies the conditions that end the action */
  virtual bool endCondition() = 0;
  /* Abort condition specifies the conditions that stop the listener of this action */
  virtual bool abortCondition() = 0;

  virtual ~Action() {};
  virtual void listen();
  virtual std::string name() const = 0;

  virtual void initBehaviours() = 0;
  const List<std::string>& getBehaviours() const {return behaviourList;}
  
  /* contains a string describing the details of the recognized action, same as print() */
  std::string info();
  
  void print();
};

std::string toString(const ActionStatus&);

#endif
