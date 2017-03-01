/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class adiministrates the actions. It contains the history of the monitored vehicle's states,
   as well as the functions to manage the specific action listeners.
*/

#ifndef ACTION_MANAGER_H
#define ACTION_MANAGER_H

#include "List.h"
#include "State.h"
#include "Action.h"

#include <iostream>

extern int now;

class ActionManager {
 private:
  /* Keeps track of the states of the monitored vehicle across the prefixed time span FIXME where? */
  List<State> monitorStates;
  /* List of actions that are being monitored. Populated during initialization. */
  List<Action*> listeners;
  /* Keeps track of the actions done by the monitored vehicle. */
  List<std::string> history;
  
 public:
  ActionManager();
  ~ActionManager();
  void init();
  void run();
  template<class T> void addListener();
  void clearListeners();
  void printHistory();
};

template<class T>
void ActionManager::addListener() {
  listeners.insHead(new T);
}




#endif
