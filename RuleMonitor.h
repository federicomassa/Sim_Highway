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
#include "SocialRules.h"
#include "ActionManager.h"
#include "Reputation.h"

#include <utility>

class RuleMonitor
{
 private:
  const ActionManager& aMan;
  Area observableArea;
  SocialRules* rules;
  Reputation& rep;
  /* every record is an action and its associated rules */
  List<std::pair<Action*, List<Rule> > > processedActions;

  void processActions();
  void registerNewAction(const Action*);
 public:
  RuleMonitor(const ActionManager&, Reputation&);
 
  /* owns social rules object */
  ~RuleMonitor();
  
  /* take ownership */
  void setRules(SocialRules* sr) {
    if (sr == NULL)
      error("RuleMonitor::setRules", "Null pointer provided");
    
    rules = sr;
    buildRules();
  }

  /* build rules */
  void buildRules();

  /* get pointer to the action manager */
  const ActionManager* getActionManager() {return &aMan;}

  Reputation& getReputation() {return rep;}
  const Reputation& getReputation() const {return rep;}
  
  /* check vehicle behaviour based on actions detected by the action manager */
  void run(const Area&);

  /* accessor to processed actions */
  const List<std::pair<Action*, List<Rule> > >& getProcessedActions() const
    {return processedActions;}
};

#endif
