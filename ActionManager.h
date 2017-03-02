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
#include "Configuration.h"

#include <fstream>

extern int now;
extern const Configuration CONF;

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

  /* clear memory */
  ~ActionManager();

  /* create listeners. */
  void init();

  /* Monitor vehicle's actions. */
  void run();

  /* For testing purposes. Take coordinates from file */
  void parseTestFile(std::ifstream&);
  
  /* allocate new object into listeners list. T must inherit from Action class. */
  template<class T> void addListener();

  /* delete objects inside listeners list and purge it. */
  void clearListeners();

  /* print vehicle's actions starting from the most recent. */
  void printHistory();
};

template<class T>
void ActionManager::addListener() {
  listeners.insHead(new T(monitorStates));
}




#endif
