#include "RuleMonitor.h"
#include "SocialRules.h"
#include "Actions.h"
#include <iostream>

RuleMonitor::RuleMonitor(const ActionManager& aM): aMan(aM) {rules = 0;}

RuleMonitor::~RuleMonitor()
{
  if (rules)
    delete rules;

  Iterator<Action*> iProcessed(processedActions);
  Action* proc;

  while (iProcessed(proc))
    delete proc;
}

void RuleMonitor::buildRules()
{
  if (rules)
    rules->build();
}

void RuleMonitor::run(const Area& obs) {
  
  observableArea = obs;
  /* error handling */
  if (rules == 0)
    error("RuleMonitor::run()", "Before checking, use setRules() method");
  
  Iterator<Action*> actionIter(*(aMan.getHistory()));
  Action* tmpA = 0;
  
  while (actionIter(tmpA))
    {
      /* First check if the action has already been processed. 
       For now actions are all evaluated at trigger time */
      Iterator<Action*> processedIter(processedActions);
      Action* processed;

      bool alreadyProcessed = false;      
      while (processedIter(processed))
	{
	  if (processed->triggerTime == tmpA->triggerTime &&
	      processed->name() == tmpA->name())
	    alreadyProcessed = true;
	    
	}

      /* if already processed, exit because older actions should have already been processed */
      if (alreadyProcessed)
	return;

      /* process every action behaviour */
      else
      	processAction(tmpA);
      
	
    }
  
}

void RuleMonitor::processAction(const Action* a)
{
  
  Iterator<std::string> iBehaviour(a->getBehaviours());
  std::string beh;

  while (iBehaviour(beh))
    {
      rules->checkRule(beh, aMan.monitorStates, aMan.neighStates, observableArea);
      processedActions.insHead(ActionManager::copyAction(a));
    }

  
}
