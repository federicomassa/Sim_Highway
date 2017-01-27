
#include "Vehicle.h"
#include <iostream>

void Vehicle::init(const State& s, const Parms& p)
{
    automaton.init(s);
    pLayer.init(s, p);
}

void Vehicle::setRM()
{
    List<Neighborhood> nList;
    mLayer.buildNeighborhoodList(nList); /* take current neighborhood list */
    repMan.setCurrentParams(pLayer.getQ(), nList);
}

void Vehicle::preRun(const List<Sensing>& sList, const Area& obs)
{
  (*this).sList = sList;
  Sensing tmpS;
  Iterator<Sensing> i(sList);
  List<State> qList;
  while(i(tmpS))
    qList.insHead(tmpS.q);
  /* detecting events */

  /* First change maneuver, then evolve pLayer*/
  automaton.run(getQ(), qList);
  
  /* physical layer evolution */
  
  pLayer.computeNextQ(automaton.getManeuver(), /* For platoon controller */qList);
  /* monitor layer evolution */
  if(mLayer.isActive())
    mLayer.run(sList, pLayer.getQ(), automaton.getManeuver(), obs);
  /* set reputation manager */
  if(repMan.isActive())
    setRM();
  
}

void Vehicle::getHypothesis(List<Hypothesis>& hList)
{
    if(mLayer.isActive())
        mLayer.getHypothesis(hList);
}

