#include "RuleMonitor.h"
#include "Actions.h"
#include <iostream>

RuleMonitor::RuleMonitor(const ActionManager& aM, Reputation& reputation): aMan(aM), rep(reputation) {rules = 0;}

RuleMonitor::~RuleMonitor()
{
  if (rules)
    delete rules;

  Iterator<std::pair<Action*, List<Rule> > > iProcessed(processedActions);
  std::pair<Action*, List<Rule> > proc;

  while (iProcessed(proc))
    delete proc.first;
}

void RuleMonitor::buildRules()
{
  if (rules)
    rules->build();
}

void RuleMonitor::run(const Area& obs) {
  ResultLog.s << "Time: " << now << EndLine(ResultLog.incrementIndentation());
  
  observableArea = obs;
  /* error handling */
  if (rules == 0)
    error("RuleMonitor::run()", "Before checking, use setRules() method");
  
  Iterator<Action*> actionIter(*(aMan.getHistory()));
  Action* tmpA = 0;
  
  while (actionIter(tmpA))
    {
      /* First see if the rule monitor has already registered this action */
      std::pair<Action*, List<Rule> >* processed;
      Action* foundAction = 0;

      for (int i = 0; i < processedActions.count(); i++)
	{
	  processedActions.getElem(processed, i);
	  /* look for a match in ActionManager history and in RuleMonitor records */
	  if (processed->first->name() == tmpA->name() &&
	      processed->first->triggerTime == tmpA->triggerTime)
	    {
	      foundAction = tmpA;
	      /* update end time */
	      processed->first->endTime = tmpA->endTime;
	      break;
	    }
	}

      /* if a match was not found */
      if (!foundAction)
	registerNewAction(tmpA);	  
      	
    }

  /* Now process actions taken by the register (processedActions list) */
  processActions();
  
}

void RuleMonitor::processActions()
{
  
  std::pair<Action*, List<Rule> >* p;

  /* list containing rules processed during this run. 
   It helps to avoid checking several time the same rule
  (different actions might share a rule) */
  List<Rule*> processedRules;
  
  /* for each action recorded */
  for (int i = 0; i < processedActions.count(); i++)
    {
      processedActions.getElem(p, i);
      
      /* if we are past the end time of that action do not check it anymore */
      if (now > p->first->endTime && p->first->endTime != -1)
	continue;

      Rule* r;
      for (int j = 0; j < p->second.count(); j++)
	{
	  p->second.getElem(r, j);

	  /* check if rule has already been processed */
	  Iterator<Rule*> processedRulesIt(processedRules);
	  Rule* procR;
	  bool ruleFound = false;
	  
	  while (processedRulesIt(procR))
	    {
	      if (*procR == *r)
		{
		  ruleFound = true;
		  break;
		}
	    }

	  if (!r->isProcessed() && !ruleFound)
	    {
	      r->check(aMan.monitorStates, aMan.neighStates, observableArea, p->first->triggerTime, p->first->endTime, getReputation());
	      processedRules.insHead(r);
	    }
	}
    }
}

void RuleMonitor::registerNewAction(const Action* a)
{
  if (a == 0)
    error("RuleMonitor::registerNewAction", "Function called with a null pointer argument");

  /* Rules will be processed by processAction method. */
  List<Rule> actionRules = rules->createRulesList(a->getRuleCategories());
  processedActions.insHead(std::make_pair(ActionManager::copyAction(a), actionRules));

}
