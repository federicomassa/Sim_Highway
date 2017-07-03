#include "Vehicle.h"
#include "Knowledge.h"
#include <iostream>
#include <utility>

void Vehicle::init(const State& s, const Parms& p)
{
  automaton.init(s);
  pLayer.init(s, p);
}

const Vehicle& Vehicle::operator=(const Vehicle& veh)
{
  idx = veh.getID();
  sList = veh.getSList();
  State tempQ = veh.getQ();
  tempQ.initManeuver = veh.getManeuver();
  automaton.init(tempQ);
  pLayer.init(veh.getQ(), veh.getParms());

  if (veh.mLayer.isActive())
    mLayer.activate();

  mLayer.setID(idx);
  // WARNING: mLayer is not yet initialized with all monitors. They will be automatically if mLayer is run.

  repMan.init(veh.repMan.getChannel(), veh.getID());

  return *this;
}

void Vehicle::setRM()
{
  Knowledge k;
  mLayer.buildKnowledge(k); /* take current neighborhood list */
  repMan.setCurrentParams(pLayer.getQ(), k);
}

void Vehicle::preRun(const List<Sensing>& sList, const Area& obs)
{
  (*this).sList = sList;
  Sensing tmpS;
  Iterator<Sensing> i(sList);
  List<State> qList;
  while (i(tmpS))
    qList.insHead(tmpS.q);
  /* detecting events */

  /* First change maneuver, then evolve pLayer*/
  automaton.run(getQ(), qList);

  /* physical layer evolution */

  pLayer.computeNextQ(automaton.getManeuver(), /* For platoon controller */qList);
}

void Vehicle::evolveMonitor(const Area& obs)
{
  /* monitor layer evolution */
  if (mLayer.isActive())
    mLayer.run(sList, pLayer.getQ(), automaton.getManeuver(), obs);
  /* set reputation manager */
  if (repMan.isActive())
    setRM();
}

void Vehicle::getHypothesis(List<Hypothesis>& hList)
{
  if (mLayer.isActive())
    mLayer.getHypothesis(hList);
}


void Vehicle::shareWaitingList()
{
  Iterator<Monitor*> mI(mLayer.getMonitorList());
  Monitor* m;

  Iterator<std::pair<int, int> > pairI(getRM().getKnowledge().monitoredVehicles);
  std::pair<int, int> p;

  while (mI(m))
  {
    while (pairI(p))
    {
      if (!(p.first == m->getTargetID()))
        continue;

      m->setTimeCount(p.second);

      break;
    };
  };

}
