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
#include "Logger.h"

#include <fstream>

extern int now;
extern const Configuration CONF;
extern Logger LOG;
extern Logger ResultLog;

class ActionManager {

  friend class RuleMonitor;
 private:
  /* Keeps track of the states of the monitored vehicle across the prefixed time span FIXME where? */
  const Vector<Sensing, VEHICLE_MEMORY>& monitorStates;
  const Vector<List<Sensing>, VEHICLE_MEMORY>& neighStates;
  /* List of actions that are being monitored. Populated during initialization. */
  List<Action*> listeners;
  /* Keeps track of the actions done by the monitored vehicle. */
  List<Action*> history;

  /* Record an ended or aborted action */
  void recordAction(Action*);

  /* Reset Action */
  void resetAction(Action*);
  
  /* delete objects inside listeners list and purge it. */
  void clearListeners();

  /* delete objects inside history list and purge it. */
  void clearHistory();

  /* Returns a pointer to a copy of the input Action* (it has to be deleted) */
  static Action* copyAction(Action*);
  static Action* copyAction(const Action*);
  
 public:
  /* Constructor */
  ActionManager(const Vector<Sensing, VEHICLE_MEMORY>&, const Vector<List<Sensing>, VEHICLE_MEMORY>&);

  /* clear memory */
  ~ActionManager();

  /* create listeners. */
  void init();

  /* Monitor vehicle's actions. */
  void run();

  /* For testing purposes. Take coordinates from file */
  void parseTestFile(std::ifstream&);
  
  /* allocate new object into listeners list. */
  void addListener(Action*);

  /* accessor to history */
  const List<Action*>* getHistory() const {return &history;}
  
  /* print vehicle's actions starting from the most recent. */
  void printHistory();

  /* get active actions */
  List<const Action*> getActiveActions() const;
};



#endif
