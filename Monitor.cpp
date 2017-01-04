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

Monitor::Monitor(int a, int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<State>& qList)
{
    agentID = a;
    targetID = t;
    targetQ = tQ;
    automaton.init(tQ);
    for(int i = 0; i < N_MANEUVER; i++)
      possibleManeuvers[i] = true;
    targetParms = tP;
    targetManeuver = tSigma;
    targetLastManeuver = UNKNOWN;
    automaton.setManeuver(tSigma, targetQ);

    //build starting neighborhood
    /* compute active area for target agent */
    Area active;
    activeArea(targetQ, active);
    
    /* determine list of ``active'' agents */
    Iterator<State> i(qList);
    State tmpQ;
    List<State> activeList;
    while(i(tmpQ))
        if(active.contains(tmpQ.toPoint()))
            activeList.insHead(tmpQ);
    
    neighbors = activeList;

    
    targetLocked = true; //FIXME: fixed to true for now
    hypReady = false;
}

void Monitor::predictStates(/* For platoon */ List<State> qList)
{
    for(int i = 0; i < N_MANEUVER; i++)
        if(possibleManeuvers[i])
        {
            Maneuver m = (Maneuver)i;
            pLayer.init(targetQ, targetParms);
            pLayer.computeNextQ(m, /* For platoon */ qList);
            pLayer.updateQ();
            possibleStates[i] = pLayer.getQ();
        }
}

void Monitor::predictManeuvers(const List<State>& qList, const Area& obs)
{ 
  /* error handling */
  if(!targetLocked)
    error("Monitor", "predictManeuvers called without target being locked");
  
  if(CONF.debug)
    {
      LOG.s << "Computing possible maneuvers for vehicle " << targetID << " as seen by vehicle " << agentID;
      LOG.s << " BEGIN..." << EndLine(EndLine::INC);
    }
  
  /* compute active area for target agent */
  Area active;
  activeArea(targetQ, active);
  
  /* determine list of ``active'' agents */
  Iterator<State> i(qList);
  State tmpQ;
  List<State> activeList;
  while(i(tmpQ))
    if(active.contains(tmpQ.toPoint()))
      activeList.insHead(tmpQ);
  
  /* updating neighbors state list */
  lastNeighbors = neighbors;
  neighbors = activeList;
  
  /* estimate events for target agent - non omniscient */
  automaton.detectEvents(targetQ, activeList, false);
  
  for(int i = 0; i < N_MANEUVER; i++)
    {
      possibleManeuvers[i] = false;
      possibleHypLists[i].purge();
    }
  
  /* compute sub-events indicator areas */
  bool valueChanged = false;
  Vector<Area, N_SUB_EVENT> subEventArea;
    Vector<int, N_SUB_EVENT> areaSign(0);
    if(CONF.debug)
      LOG.s << "SubEvents visit:" << EndLine();
    
    
    for(int k = 0; k < N_SUB_EVENT; k++)
      {
        SubEvent& se = automaton.subEvents[k];
        if(CONF.debug)
	  {
            LOG.s << se << EndLine(EndLine::INC);
            LOG.s << "Evaluation time: " << se.evalTime;
            LOG.s << EndLine() << "Value: " << se.value;
	  }
        if(se.value.nonOmniscientValue == U && se.evalTime == now)
	  {
            if(se.areaFunc)
	      {
                /* compute sub-event indicator area */
                Area indicator;
                se.areaFunc(targetQ, indicator);
                /* subtract observable area */
                indicator -= obs;
                if(indicator.isEmpty())
		  {
                    if(se.mode == OR)
		      /* there isn't any agent where there should be */
		      se.value.nonOmniscientValue = F;
                    else
		      /* there isn't any agent where there shouldn't be */
		      se.value.nonOmniscientValue = T;
                    valueChanged = true;
                    if(CONF.debug)
                        LOG.s << EndLine() << "Value changed: " << se.value;
		  }
                else
		  {
                    subEventArea[k] = indicator;
                    if(se.mode == OR)
		      areaSign[k] = 1;
                    else
		      areaSign[k] = -1;
		  }
	      }
	  }
        if(CONF.debug)
	  LOG.s << EndLine(EndLine::DEC);
      }
    /* sub-events re-evaluation */
    if(valueChanged)
      {
        /* at least one sub-event value changed */
        if(CONF.debug)
	  LOG.s << "SubEvents second pass:" << EndLine();
        for(int i = 0; i < N_MANEUVER; i++)
	  automaton.transitions[targetManeuver][i].reEvaluate();
      }
    /* check for certain events */
    bool certainEventDetected = false;
    // if all transitions are false, then vehicle should keep the same maneuver
    ExtBool sameManeuverDetected = F;
    for(int i = 0; i < N_MANEUVER; i++)
      {
	if (targetManeuver == (Maneuver)i)
	  continue;
	
        Transition& t = automaton.transitions[targetManeuver][i];
	sameManeuverDetected = sameManeuverDetected || t.getValue().nonOmniscientValue;
        if(t.getValue().nonOmniscientValue == T)
        {
            possibleManeuvers[i] = true;
            Iterator<Event*> ei(t.eventList);
            Event* e;
            while(ei(e))
                if(e->getValue().nonOmniscientValue == T)
                {
                    /* build Hypothesis without uncertainty */
                    Hypothesis h;
                    h.eventID = e->idx;
                    possibleHypLists[i].insHead(h);
                    break;
                }
            certainEventDetected = true;
            break;
        }
	
      }

    // NOR
    sameManeuverDetected = !sameManeuverDetected;

    
    if (sameManeuverDetected == T)
      {
	// SIGMA => SIGMA detected: build hypothesis without uncertainty
	Hypothesis h;
	possibleHypLists[(int)targetManeuver].insHead(h);
	certainEventDetected = true;
      }
    
    if(!certainEventDetected)
        /* hypothesis construction */
      for(int i = 0; i < N_MANEUVER; i++)
        {

	  possibleHypLists[i].purge();
	  
	  if (targetManeuver == (Maneuver)i)
	    {
	      if (sameManeuverDetected == U)
		{
		  Hypothesis h;
		  h.isSameManeuverUncertain = true;
		  possibleHypLists[i].insHead(h);
		}
	      continue;
	    }
	  
	  Transition& t = automaton.transitions[targetManeuver][i];
	  if(t.getValue().nonOmniscientValue == U)
            {
	      possibleManeuvers[i] = true;
	      Iterator<Event*> ie(t.eventList);
	      Event* e;
	      while(ie(e))
		if(e->value.nonOmniscientValue == U)
		  {
		    Hypothesis h;
		    h.eventID = e->idx;
		    Iterator<SubEvent*> ise(e->subEventList);
		    SubEvent* se;
		    while(ise(se))
		      if(se->value.nonOmniscientValue == U)
			{
			  int j = se->idx;
			  if(areaSign[j] == 1)
			    {
			      Hypothesis::SubHypothesis sh;
			      sh.subEventID = j; //!!!! Changed from i to j
			      sh.positive = subEventArea[j];
			      h.subHypList.insHead(sh);
			    }
			  else /* areaSign[j] == -1 */
			    {
			      h.negative += subEventArea[j];
			    }
			}
		    /* add generated hypothesis */
		    possibleHypLists[i].insHead(h);
		    LOG.s << "Added hypothesis on transition: " << targetManeuver << " => " << (Maneuver)i;
		    if (!h.subHypList.isEmpty())
		      LOG.s << " with subhypotheses...";
		    if (!h.negative.isEmpty())
		      LOG.s << " with negative area...";
		    LOG.s << EndLine();
		  }
            }
        }
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC);
	LOG.s << "Computing possible maneuvers for vehicle " << targetID << " as seen by vehicle " << agentID;
        LOG.s << " END." << EndLine();
    }
}

void Monitor::detectManeuver(const State& q, const Maneuver& sigma)
{
  /* reset hypothesis */
  hypothesisList.purge();

  targetLastManeuver = targetManeuver;
  lastHypLists = possibleHypLists;
  
  if(CONF.debug)
    LOG.s << "Detected maneuver " << sigma << EndLine();
  if(targetLocked)
    {
      /* now Neighborhood object can be build */
      hypothesisList = possibleHypLists[(int)sigma];
      hypReady = true;
    }

  automaton.setManeuver(sigma, q);   
  
  if(sigma != targetManeuver || !CONF.monitorsNeedLock)
    {
      if(CONF.debug && CONF.monitorsNeedLock)
	LOG.s << "Transition detected (target locked)" << EndLine();
      /* transition detected */
      targetManeuver = sigma;

      /* got a good estimation of target xi */
      targetLocked = true;
    }
  
  hypothesisList.sort();
  targetLastQ = targetQ;
  targetQ = q;
}

bool Monitor::getFirstHypothesis(Hypothesis& hyp) const
{
    return hypothesisList.getFirst(hyp);
}

bool Monitor::buildNeighborhood(Neighborhood& n) const
{
    if(!hypReady)
        return false;
    
    n.targetID = targetID;
    n.agentID = agentID;
    n.qTarget = targetLastQ;
    n.qList = lastNeighbors;
    n.targetManeuver = targetManeuver;
    n.targetLastManeuver = targetLastManeuver;
    n.hList = hypothesisList;
    n.lastHypLists = lastHypLists;
    
    return true;
}
