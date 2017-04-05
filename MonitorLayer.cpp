/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        MonitorLayer.cpp
 * DESCRIPTION: Implementation of the MonitorLayer class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "MonitorLayer.h"
#include "Environment.h"
#include <utility>

MonitorLayer::~MonitorLayer()
{
    /* delete the monitors in the list */
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        delete mon;

    if (currentEnv)
      delete currentEnv;
}

Monitor* MonitorLayer::addMonitor(int t)
{
  Monitor* mon = new Monitor(agentID, t);
  monitorList.insHead(mon);
  return mon;
}

Monitor* MonitorLayer::lookFor(int t)
{
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        if(mon->getTargetID() == t)
            return mon;

    return NULL;
}

const Monitor* MonitorLayer::getMonitor(const int& id) const
{
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        if(mon->getTargetID() == id)
            return mon;

    return NULL;
}


bool MonitorLayer::removeMonitor(int t)
{
    Monitor* m = lookFor(t);
    if(m == NULL)
        return false;

    monitorList.delInfo(m);
    delete m;

    return true;
}

void MonitorLayer::run(const List<Sensing>& sList, const State& agentQ, const Parms& agentP, const Maneuver& agentManeuver,
                       const Area& obs)
{ 
  if(CONF.debug)
    {
      LOG.s << "Monitor Layer " << agentID << " START...";
      LOG.s << EndLine(EndLine::INC);
    }
  
  /* Create environment to calculate observable areas of monitored vehicles based on 
     agent's partial knowledge */

  if (currentEnv)
    delete currentEnv;
  
  currentEnv = new Environment(sList.count() + 1, CONF.cRadius, CONF.cProb);
  Iterator<Sensing> i(sList);
  Sensing s;

  agentStates.insHead(agentQ);
  agentNeighStates.insHead(sList);

  List<std::pair<State, Parms> > totalPList;

  while (i(s))
    {
      State q(s.x, s.y, s.theta, s.v);
      /* some parameters are not measured but they are not relevant -> set default values */
      Parms p(s.desiredV, FAST, s.vehicleType, "StandardUnicycle", "StandardRules");
      totalPList.insTail(std::make_pair(q,p));
    }

  /* add yourself to the environment, it will be the last on the list */
  totalPList.insTail(std::make_pair(agentQ, agentP));
  
  currentEnv->initVehicles(totalPList);
  
  const Sensing* sens;
  for (int n = 0; n < currentEnv->getNVehicles() - 1; n++)
    {
      /* Using the fact that the vehicles are ordered as sList, except agent vehicle which is at the bottom of the list */
      sList.getElem(sens, n);
      currentEnv->getVehicles()[n].setID(sens->agentID);
    }

  /* set my ID */
  currentEnv->getVehicles()[currentEnv->getNVehicles() - 1].setID(agentID);
  
  /* Reset iterator */
  i.initialize(sList);
  
  List<int> processedID;
  while(i(s))
    {
      /* check if the target id is in the active targets list */
      if(!CONF.allTargetsActive && !CONF.activeTargets.belongs(s.agentID))
	continue;
      
      /* Look for correspondence between vehicles index in environment and in sList */
      int monitorIndex = -1;
      for (int n = 0; n < currentEnv->getNVehicles(); n++)
	{
	  if (currentEnv->getVehicles()[n].getID() == s.agentID)
	    {
	      monitorIndex = n;
	      break;
	    }
	}
      
      processedID.insHead(s.agentID);
      
      // building state list, as seen by monitored vehicle
      Iterator<Sensing> is(sList);
      Sensing tmpS;
      List<Sensing> targetSList;
      
      Area monitorObs;
      currentEnv->observableArea(monitorIndex, monitorObs);
      
      while(is(tmpS))
	{
	  /* do not add itself to the list */
	  if (tmpS == s)
	    continue;
	   
	  
	  /* search corresponding index in vehicles list in environment */
	  int tmpIndex = -1;
	  for (int n = 0; n < currentEnv->getNVehicles(); n++)
	    {
	      if (currentEnv->getVehicles()[n].getID() == tmpS.agentID)
		{
		  tmpIndex = n;
		  break;
		}
	    }
	  
	  if (currentEnv->getVehicles()[tmpIndex].inArea(monitorObs))
	    targetSList.insHead(tmpS);
	}
      // insert monitor agent state in the list
      Sensing agentS(agentID, agentQ, agentP);
      targetSList.insHead(agentS);
      
      Monitor* m = lookFor(s.agentID);
      if(m == NULL)
        {
	  m = addMonitor(s.agentID);
	}
      
      
      //else
      
      /* detect target maneuver 
	 m->detectManeuver(s.q, s.sigma);
      */
      /* predict possible maneuvers 
	 m->predictManeuvers(qList, obs);
      */
      
      /* Prediction --- FIXME Testing for now */
      /* Call after detectManeuver to update targetQ */

      // =================================== //
      /* Here starts the actual monitor*/
      
      monitorLog.s << "Vehicle " << agentID << " observing vehicle " << m->getTargetID() << " BEGIN..." << EndLine(EndLine::INC);

      /* run the monitor passing the last points recorded. The monitor will form its own neighbors */
      if (agentID == 0 && s.agentID == 1)
	m->run(s, targetSList, obs);
      
      monitorLog.s << EndLine(EndLine::DEC);
      monitorLog.s << "Vehicle " << agentID << " observing vehicle " << m->getTargetID() << " END..." << EndLine();
      
    }
  
  if(CONF.debug)
    LOG.s << "Deleting unused monitors: ";
  /* deleting unused monitors */
  Iterator<Monitor*> im(monitorList);
  Monitor* m;
  while(im(m))
    if(!processedID.belongs(m->getTargetID()))
      {
	if(CONF.debug)
	  LOG.s << m->getAgentID() << ' ';
	monitorList.delInfo(m);
	delete m;
      }
  if(CONF.debug)
    {
      LOG.s << EndLine(EndLine::DEC);
      LOG.s << "Monitor Layer " << agentID << " STOP.";
    }

}

void MonitorLayer::getHypothesis(List<Hypothesis>& hList) const
{
  
    /* error handling */
  /*
    if(hList.count() > 0)
        error("MonitorLayer", "Hypothesis list MUST be empty");
    
    Iterator<Monitor*> i(monitorList);
    Monitor* m;
    while(i(m))
    {
        Hypothesis hyp;
        if(m->getFirstHypothesis(hyp))
            hList.insHead(hyp);
	    }*/
}

void MonitorLayer::buildNeighborhoodList(List<Neighborhood>& nList) const
{
    /* error handling */
    if(nList.count() > 0)
        error("MonitorLayer", "Neighborhood list MUST be empty");
    
    Iterator<Monitor*> i(monitorList);
    Monitor* m;
    while(i(m))
    {
        Neighborhood n;
	/*        if(m->buildNeighborhood(n))
	  {
            nList.insHead(n);
	    }*/
    }


    
    
}

void MonitorLayer::buildKnowledge(Knowledge& k) const
{
  List<Neighborhood> nList;
  List<std::pair<int, int> > monitoredVehicles;
  
  Iterator<Monitor*> i(monitorList);
  Monitor* m;
  while(i(m))
    {
      Neighborhood n;
      /*      if(m->buildNeighborhood(n))
	{
	  nList.insHead(n);
	  }*/
      
      /*      std::pair<int, int> p = std::make_pair(m->targetID, m->getTimeCount());
	      monitoredVehicles.insHead(p);*/
    }


  k.nList = nList;
  k.monitoredVehicles = monitoredVehicles;
}
