/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   DATE = 05/02/2017
   DESCRIPTION = Object that contains the knowledge available to each vehicle that will be shared with the others

*/

#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include "Neighborhood.h"
#include "Area.h"
#include <utility>
#include <iostream>

/* The message that will be sent to the other vehicles is something like: "I am monitoring vehicle 1 and 2, 
   it still takes respectively 4 and 5 time steps to complete the measurement of their discrete state", or,
   if finished, send the neighborhood list as measured. */
class Knowledge
{
  friend std::ostream& operator<<(std::ostream& os, const Knowledge& k);
 public:
  Knowledge();
  /* pairs containing monitored vehicle's ID and a countdown indicating how long it will take to attempt to measure the 
   discrete state. */
  int agentID;
  Area obs;
  List<std::pair<int, int> > monitoredVehicles;

  /* Neighborhood list: sent only when the discrete state measurement is complete */
  List<Neighborhood> nList;
  
  const int& getTargetID() const {return agentID;}
};

#endif
