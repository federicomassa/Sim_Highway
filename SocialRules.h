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
extern Logger ResultLog;

class SocialRules;

/* Type that describes when the rule has to be evaluated:
 ex: at trigger time or at each instant */
enum CheckMode{TRIGGER, CONTINUOUS};

class Rule {
  friend class SocialRules;

 private:
  
  /* This is the list of events that describes the forbidden situations for this rule category.
     In the case of the category "LeftLaneChange", an event could be like "left lane is occupied by someone else and you are not on 
     the maximum lane" */
  List<Event> eList;
  int lastCheckTime;
  bool processed;

  void updateProcessStatus(const int&, const int&);

  /* This is the name of the category that we want to set a rule on.
     This is very important as each rule set a list of situations in which
     this behaviour should not happen. An example of category could be "LeftLaneChange".
     Each action, as recognized by the ActionManager, is possibly registered to several categories. */
  std::string category;
  std::string name;
  CheckMode mode;
  
  
  void init(const std::string& cat, const List<Event>& eL, const std::string& n, const CheckMode& m);
 public:

  Rule();
  
  const List<Event>& getEventList() {return eList;}
  
  void check(const Vector<Sensing, 10>& monitorStates, const Vector<List<Sensing>, 10>& neighStates, const Area& obs, const int& triggerTime,
	     const int& endTime);
  
  bool isProcessed() const;
  
  bool operator==(const Rule& r)
  {
    return (category == r.category &&
	    name == r.name &&
	    eList == r.eList);
	    
  }

  bool operator!=(const Rule& r) {return !((*this) == r);}

  const std::string& getCategory() const {return category;}
};

class SocialRules
{
  
 protected:
  List<Rule> rList;
  
  /* Add rule to the list */
  void addRule(const Rule& r) {rList.insHead(r);}
  void addRule(const std::string& cat, const List<Event>& eL, const std::string& name, const CheckMode& mode = TRIGGER)
  {
    Rule r;
    r.init(cat, eL, name, mode);
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

  List<Rule> createRulesList(const List<std::string>&);
  
};


#endif
