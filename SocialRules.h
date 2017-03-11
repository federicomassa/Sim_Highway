/* 
   AUTHOR = Federico Massa, Adriano Fagiolini,
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes a social rule. Many rules together constitutes the set of social rules that has to be monitored.
*/

#ifndef SOCIAL_RULES_H
#define SOCIAL_RULES_H

#include "List.h"
#include "State.h"
#include "Area.h"
#include "EndLine.h"
#include "Event.h"
#include "RuleMonitor.h"
#include <string>

class SocialRules;

class Rule {
  friend class SocialRules;
 private:
  
  /* This is the list of events that describes the forbidden situations for this behaviour.
     In the case of the behaviour "LeftLaneChange", an event could be like "left lane is occupied by someone else" */
  List<Event> eList;
  
 public:
  Rule() {}

  /* This is the name of the behaviour that we want to set a rule on.
     This is very important as each rule set a list of situations in which
     this behaviour should not appear. An example of behaviour could be "LeftLaneChange".
     Each action, as recognized by the ActionManager, is registered to a number of behaviours. */
  std::string behaviour;
  std::string name;
  const List<Event>& getEventList() {return eList;}
  
  void init(const std::string& beh, const List<Event>& eL, const std::string& n)
  {
    behaviour = beh;
    eList = eL;
    name = n;
  }

  /*virtual */void check(const Vector<State, 10>& monitorStates, const Vector<List<State>, 10>& neighStates, const Area& obs)
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
	std::cout << "Evaluating rule: " << name << EndLine(EndLine::INC);
	e.evaluateWithArea(monitorStates[0], 0.0, neighStates[0], false, obs);
	std::cout << "Result: " << e.getValue().nonOmniscientValue << EndLine(EndLine::DEC);
      }

    
    
  }
  
  bool operator==(const Rule& r)
  {
    return (behaviour == r.behaviour &&
	    eList == r.eList);
	    
  }

  bool operator!=(const Rule& r) {return !((*this) == r);}
  
};

class SocialRules
{
  
 protected:
  List<Rule> rList;
  
  /* Add rule to the list */
  void addRule(const Rule& r) {rList.insHead(r);}
  void addRule(const std::string& beh, const List<Event>& eL, const std::string& name)
  {
    Rule r;
    r.init(beh, eL, name);
    addRule(r);
  }

  /* FIXME Virtual template version of addRule if one wants to implement his version of check? */
  /* template <class Derived>
   addRule(const Derived& r)
  we would need not a List<Rule> but a RefList<Rule>, where RefList is a List that contains references without instantiating objects. */
  
  /* Accessor (by reference) */
  const List<Rule>& getRules() {return rList;}
  
  /* clear list */
  void purge() {rList.purge();}
  
 public:
  SocialRules() {}
  virtual ~SocialRules() {}
  /* build list of rules. The memory for the sub-events must be allocated here
   and will be freed by the destructor */
  virtual void build() = 0;

  /* Check rule corresponding to the desired behaviour */
  void checkRule(const std::string& behaviour, const Vector<State, 10>& monitorStates, const Vector<List<State>, 10>& neighStates, const Area& obs)
  {
    Iterator<Rule> iRule(rList);
    Rule r;
    
    while (iRule(r))
      {
	if (r.behaviour == behaviour)
	  {
	    r.check(monitorStates, neighStates, obs);
	  }
      }
    
  }

  
};


#endif
