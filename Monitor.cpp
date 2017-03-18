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
#include "Predictor.h"
#include "Grid.h"
#include "Logger.h"
#include "TestRules.h"

#include <iostream>

Monitor::Monitor(int a, int t) : aMan(monitorStates, neighStates), rMon(aMan)
{
    agentID = a;
    targetID = t;

    aMan.init();
    rMon.setRules(new TestRules);
}

void Monitor::run(const State& targetQ, const List<Sensing>& targetSList, const Area& monitorObs)
{
  /* update memory with new info */
  monitorStates.insHead(targetQ);
  neighStates.insHead(targetSList);
  
  aMan.run();
  rMon.run(monitorObs);

  aMan.printHistory();
  
}



