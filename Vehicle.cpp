#include "Vehicle.h"
#include "Knowledge.h"
#include <iostream>
#include <utility>
#include <fstream>


void Vehicle::init(const std::pair<State, Parms>& sp)
{
    automaton.init(sp);
    pLayer.init(sp);
    p = sp.second;
}

void Vehicle::setRM()
{
}

void Vehicle::preRun(const List<Sensing>& sList, const Area& obs)
{
  (*this).sList = sList;
  
  Sensing myS(idx, getQ(), getParms());
  /* First change maneuver, then evolve pLayer*/    
  automaton.run(myS, sList);
  
  /* physical layer evolution */
  pLayer.computeNextQ(automaton.getManeuver(), sList);

}

void Vehicle::evolveMonitor(const Area& obs)
{
  /* monitor layer evolution */
  if(mLayer.isActive())
    mLayer.run(sList, pLayer.getQ(), getParms(), automaton.getManeuver(), obs);
  /* set reputation manager */
  if(repMan.isActive())
    setRM(); 
}

void Vehicle::getHypothesis(List<Hypothesis>& hList)
{
  /*    if(mLayer.isActive())
        mLayer.getHypothesis(hList);*/
}


void Vehicle::shareWaitingList()
{
  /*Iterator<Monitor*> mI(mLayer.getMonitorList());
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
  */
}
