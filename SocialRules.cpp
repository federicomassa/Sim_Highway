#include "SocialRules.h"
#include "utility.h"

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

List<Rule> SocialRules::createRulesList(const List<std::string>& behaviourList)
{
  Iterator<std::string> bIt(behaviourList);
  std::string beh;

  List<Rule> actionRules;
  
  /* for each behaviour */
  while (bIt(beh))
    {
      Iterator<Rule> rIt(rList);
      Rule r;

      /* for each rule composing the social rules */
      while (rIt(r))
	{
	  if (r.behaviour == beh)
	    actionRules.insHead(r);
	}
    }

  return actionRules;
  
}
