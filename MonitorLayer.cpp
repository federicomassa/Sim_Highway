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

bool MonitorLayer::removeMonitor(int t)
{
    Monitor* m = lookFor(t);
    if(m == NULL)
        return false;

    monitorList.delInfo(m);
    delete m;

    return true;
}

void MonitorLayer::run(const List<Sensing>& sList, const State& agentQ, const Maneuver& agentManeuver,
                       const Area& obs)
{ 
  if(CONF.debug)
    {
      LOG.s << "Monitor Layer " << agentID << " START...";
      LOG.s << EndLine(EndLine::INC);
    }
  
  /* Create environment to calculate observable areas of monitored vehicles based on 
     agent's partial knowledge */
  
  Environment env(sList.count(), CONF.cRadius, CONF.cProb);
  List<State> totalQList;
  List<Parms> totalPList;
  Iterator<Sensing> i(sList);
  Sensing s;

  agentStates.insHead(agentQ);
  agentNeighStates.insHead(sList);
  
  while (i(s))
    {
      totalQList.insHead(s.q);
      totalPList.insHead(s.q.v);      
    }
  
  env.initVehicles(totalQList, totalPList);
  
  const Sensing* sens;
  for (int n = 0; n < env.getNVehicles(); n++)
    {
      sList.getElem(sens, n);
      env.getVehicles()[n].setID(sens->agentID);
    }
  
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
      for (int n = 0; n < env.getNVehicles(); n++)
	{
	  if (env.getVehicles()[n].getID() == s.agentID)
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
      env.observableArea(monitorIndex, monitorObs);
      
      while(is(tmpS))
	{
	  /* do not add itself to the list */
	  if (tmpS == s)
	    continue;
	   
	  
	  /* search corresponding index in vehicles list in environment */
	  int tmpIndex = -1;
	  for (int n = 0; n < env.getNVehicles(); n++)
	    {
	      if (env.getVehicles()[n].getID() == tmpS.agentID)
		{
		  tmpIndex = n;
		  break;
		}
	    }
	  
	  if (env.getVehicles()[tmpIndex].inArea(monitorObs))
	    targetSList.insHead(tmpS);
	}
      // insert monitor agent state in the list
      Sensing agentS(agentID, agentQ, agentQ.v, UNKNOWN);
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
	m->run(s.q, targetSList, obs);
      
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
