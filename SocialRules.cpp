#include "SocialRules.h"
#include "utility.h"
#include "Reputation.h"

Rule::Rule()
{
  lastCheckTime = -1;
  processed = false;
}

void Rule::check(const Vector<Sensing, VEHICLE_MEMORY>& monitorStates, const Vector<List<Sensing>, VEHICLE_MEMORY>& neighStates, const Area& obs, const int& triggerTime,
		 const int& endTime, Reputation& rep)
{
  Iterator<Event> eIt(eList);
  Event e;

  checkResult = F;
  
  /* The evaluation of the event is on the current state. See comments to template addRule!
     If we want to do something more complicated for specific rules we need to derive from rule our version of check() */
  while (eIt(e))
    {
      List<Area> positiveArea;
      Area negativeArea;
      e.evaluateWithArea(monitorStates[0], 0.0, neighStates[0], false, obs, positiveArea, negativeArea);
      rep.addRecord(now, name, e.getValue().nonOmniscientValue, positiveArea, negativeArea);
      checkResult = checkResult || e.getValue().nonOmniscientValue;
 
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
