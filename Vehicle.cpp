#include "Vehicle.h"
#include "Knowledge.h"
#include <iostream>
#include <utility>
#include <fstream>

Vehicle::Vehicle()
{
  idx = -1;
}

Vehicle::~Vehicle()
{}

void Vehicle::init(const std::pair<State, Parms>& sp)
{
    automaton.init(sp);
    
    pLayer.init(sp);
    p = sp.second;    

}

void Vehicle::setID(int index)
{
  idx = index;
  automaton.setID(index);
  mLayer.setID(index);
    
  pLayer.setID(index);
}

const Vehicle& Vehicle::operator=(const Vehicle& v)
{
  std::pair<State, Parms> sp = std::make_pair(v.getQ(), v.getParms());
  setID(v.getID());
  init(sp);

  return *this;
}

State Vehicle::getQ() const
{
  return pLayer.getQ();
}

Sensing Vehicle::getS() const
{
  State q = getQ();
  Parms p = getParms();
  
  Sensing s(idx, q, p);

  return s;
}

/* set continuous state q - for predictor */
void Vehicle::setQ(const State& q)
{
  pLayer.setQ(q);
}

bool Vehicle::inArea(const Area& a) const
{  
  return a.contains(pLayer.getQ().toPoint());
}

void Vehicle::run()
{  
  pLayer.updateQ();
}

void Vehicle::setRM(const Area& obs)
{
  Knowledge k;
  mLayer.buildKnowledge(k, obs);
  repMan.setCurrentParams(pLayer.getQ(), k);
  repMan.setReputations(mLayer);
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
    {
      mLayer.run(sList, pLayer.getQ(), getParms(), automaton.getManeuver(), obs);
    }
  /* set reputation manager */
  if(repMan.isActive())
    {
      setRM(obs);
    }
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
