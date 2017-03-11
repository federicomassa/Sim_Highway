/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Each monitor has a RuleMonitor object that checks if the actions detected by the ActionManager are
   compatible with the set of social rules.
*/

#ifndef RULE_MONITOR_H
#define RULE_MONITOR_H

#include "List.h"
#include "State.h"
#include "Area.h"
#include "ActionManager.h"

class SocialRules;

class RuleMonitor
{
 private:
  const ActionManager& aMan;
  Area observableArea;
  SocialRules* rules;
  List<Action*> processedActions;

  void processAction(const Action*);
 public:
  RuleMonitor(const ActionManager&);
 
  /* owns social rules object */
  ~RuleMonitor();
  
  /* take ownership */
  void setRules(SocialRules* sr) {rules = sr; buildRules();}

  /* build rules */
  void buildRules();

  /* get pointer to the action manager */
  const ActionManager* getActionManager() {return &aMan;}
  
  /* check vehicle behaviour based on actions detected by the action manager */
  void run(const Area&);
};

#endif
