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
#include <string>

extern int now;

class SocialRules;

/* Type that describes when the rule has to be evaluated:
 ex: at trigger time or at each instant */

enum CheckMode{TRIGGER, CONTINUOUS};

class Rule {
  friend class SocialRules;

 private:
  
  /* This is the list of events that describes the forbidden situations for this behaviour.
     In the case of the behaviour "LeftLaneChange", an event could be like "left lane is occupied by someone else and you are not on 
     the maximum lane" */
  List<Event> eList;
  int lastCheckTime;
  bool processed;

  void updateProcessStatus(const int&, const int&);

  /* This is the name of the behaviour that we want to set a rule on.
     This is very important as each rule set a list of situations in which
     this behaviour should not happen. An example of behaviour could be "LeftLaneChange".
     Each action, as recognized by the ActionManager, is possibly registered to several behaviours. */
  std::string behaviour;
  std::string name;
  CheckMode mode;
  
  
  void init(const std::string& beh, const List<Event>& eL, const std::string& n, const CheckMode& m)
  {
    behaviour = beh;
    eList = eL;
    name = n;
    mode = m;
  }
 public:

  Rule()
    {
      lastCheckTime = -1;
      processed = false;
    }

  const List<Event>& getEventList() {return eList;}
  
  void check(const Vector<State, 10>& monitorStates, const Vector<List<State>, 10>& neighStates, const Area& obs, const int& triggerTime,
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
	std::cout << "Evaluating rule: " << name << EndLine(EndLine::INC);
	e.evaluateWithArea(monitorStates[0], 0.0, neighStates[0], false, obs);
	std::cout << "Result: " << e.getValue().nonOmniscientValue << EndLine(EndLine::DEC);
      }

    lastCheckTime = now;

    updateProcessStatus(triggerTime, endTime);
    
  }

  bool isProcessed() const;
  
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
  void addRule(const std::string& beh, const List<Event>& eL, const std::string& name, const CheckMode& mode = TRIGGER)
  {
    Rule r;
    r.init(beh, eL, name, mode);
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
  /*  void checkRule(const std::string& behaviour, const Vector<State, 10>& monitorStates, const Vector<List<State>, 10>& neighStates, const Area& obs, const int& triggerTime, const int& endTime, bool& actionProcessed)
  {
    Iterator<Rule> iRule(rList);
    Rule r;
  */
    /* processed remains true if every rule has been completely evaluated (AND operation). 
     There are rules that only need to be evaluated at trigger time, others should be evaluated throughout the
    whole action. */
  /*    actionProcessed = true;
    
	while (iRule(r))
      {
	if (r.behaviour == behaviour)
	  {
	    const bool ruleProcessed = r.isProcessed(triggerTime, endTime);
	    r.check(monitorStates, neighStates, obs);
	    actionProcessed = actionProcessed && ruleProcessed;
	    }
	  }
    
  }
  */
  List<Rule> createRulesList(const List<std::string>&);
  
};


#endif
