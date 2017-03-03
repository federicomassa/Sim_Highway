/* 
   AUTHOR = Federico Massa, Adriano Fagiolini,
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = This class describes a social rule. Many rules together constitutes the set of social rules that has to be monitored.
*/

#ifndef SOCIAL_RULES_H
#define SOCIAL_RULES_H

#include "List.h"
#include "Event.h"
#include <string>

struct Rule {
  Rule(const std::string& beh, const List<Event>& eL) : behaviour(beh), eList(eL) {}
  
  /* This is the name of the behaviour that we want to set a rule on.
   This is very important as each rule set a list of situations in which
  this behaviour should not appear. An example of behaviour could be "LeftLaneChange".
  Every action, as recognized by the ActionManager, is registered to a number of behaviours. */
  const std::string behaviour;
  
  /* This is the list of events that describes the forbidden situations for this behaviour.
     In the case of the behaviour "LeftLaneChange", an event could be like "left lane is occupied by someone else" */
  const List<Event> eList;
};

class SocialRules
{
 private:
  List<Rule> rList;
  
 protected:
  /* Add rule to the list */
  void addRule(const Rule& r) {rList.insHead(r);}
  
  /* clear list */
  void purge() {rList.purge();}
  
 public:
  SocialRules() {}
  
  /* build list of rules. Only for daughters */
  virtual void build() = 0;
};


#endif
