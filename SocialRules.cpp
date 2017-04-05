#include "SocialRules.h"
#include "utility.h"

Rule::Rule()
{
  lastCheckTime = -1;
  processed = false;
}

void Rule::check(const Vector<Sensing, 10>& monitorStates, const Vector<List<Sensing>, 10>& neighStates, const Area& obs, const int& triggerTime,
		 const int& endTime)
{
  Iterator<Event> eIt(eList);
  Event e;
  
  /* The evaluation of the event is on the current state. See comments to template addRule!
     If we want to do something more complicated for specific rules we need to derive from rule our version of check() */
  while (eIt(e))
    {
      /* Debug */
      /*	e.evaluate(monitorStates[0], 0.0, neighStates[0], false);
		std::cout << "Value before area: " << e.getValue() << std::endl;*/
      ResultLog.s << "Evaluating rule: " << name << EndLine(ResultLog.incrementIndentation());
      e.evaluateWithArea(monitorStates[0], 0.0, neighStates[0], false, obs);
      ResultLog.s << "Result: " << e.getValue().nonOmniscientValue << EndLine(ResultLog.decrementIndentation());
    }
  
  
  
  lastCheckTime = now;
  
  updateProcessStatus(triggerTime, endTime);
  
}


void Rule::init(const std::string& cat, const List<Event>& eL, const std::string& n, const CheckMode& m)
{
  category = cat;
  eList = eL;
  name = n;
  mode = m;
}


void Rule::updateProcessStatus(const int& triggerTime, const int& endTime)
{
  switch(mode)
    {
      /* evaluation at trigger time */
    case TRIGGER:
      if (lastCheckTime >= triggerTime)
	processed = true;
      break;
      
      /* continuous evaluation until the end of the action */
    case CONTINUOUS:
      if (lastCheckTime >= endTime && endTime != -1)
	processed = true;
      break;
      
    default:
      error("Rule::isProcessed", "This rule's CheckMode was not recognized"); 
    }

}

bool Rule::isProcessed() const {return processed;}

List<Rule> SocialRules::createRulesList(const List<std::string>& ruleCategoryList)
{
  Iterator<std::string> cIt(ruleCategoryList);
  std::string cat;

  List<Rule> actionRules;
  
  /* for each behaviour */
  while (cIt(cat))
    {
      Iterator<Rule> rIt(rList);
      Rule r;

      /* for each rule composing the social rules */
      while (rIt(r))
	{
	  if (r.category == cat)
	    actionRules.insHead(r);
	}
    }

  return actionRules;
  
}
