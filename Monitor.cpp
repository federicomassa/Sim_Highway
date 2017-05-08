/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Monitor.cpp
 * DESCRIPTION: Implementation of the Monitor class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Monitor.h"
#include "Logger.h"
#include "TestRules.h"

#include <iostream>

Monitor::Monitor(int a, int t) : aMan(a, t, monitorStates, neighStates), rMon(aMan, rep)
{
    agentID = a;
    targetID = t;
    rep.targetID = t;
    
    aMan.init();
    rMon.setRules(new TestRules);
}

void Monitor::run(const Sensing& targetQ, const List<Sensing>& targetSList, const Area& monitorObs)
{
  /* if monitor disappeared for a while */
  if (now - lastRunTime != 1)
    {
      /* add empty elements to the vectors */
      for (int i = 0; i < (now - lastRunTime - 1); i++)
	{
	  Sensing dummyS;
	  List<Sensing> dummySList;

	  monitorStates.insHead(dummyS);
	  neighStates.insHead(dummySList);
	}
    }
  
  /* update memory with new info */
  monitorStates.insHead(targetQ);
  neighStates.insHead(targetSList);

  aMan.run();
  rMon.run(monitorObs);
  
  aMan.printHistory();

  lastRunTime = now;
}

bool Monitor::buildNeighborhood(Neighborhood& n)
{
  if (neighStates[0].count() == 0)
    return false;

  if (monitorStates[0].dummy)
    error("Monitor::buildNeighborhood", "Found dummy object on top of the vector");

  n.init(targetID, monitorStates[0], neighStates[0]);
  
  return true;
}

