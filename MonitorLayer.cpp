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

MonitorLayer::~MonitorLayer()
{
    /* delete the monitors in the list */
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        delete mon;
}

Monitor* MonitorLayer::addMonitor(int t, const State& tQ, const Parms& tP)
{
    Monitor* mon = new Monitor(agentID, t, tQ, tP);
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

void MonitorLayer::run(const List<Sensing>& sList, const State& agentQ,
                       const Area& obs)
{
    if(CONF.debug)
    {
        LOG.s << "Monitor Layer " << agentID << " START...";
        LOG.s << EndLine(EndLine::INC);
    }
    Iterator<Sensing> i(sList);
    Sensing s;
    List<int> processedID;
    while(i(s))
    {
        /* check if the target id is in the active targets list */
        if(!CONF.allTargetsActive && !CONF.activeTargets.belongs(s.agentID))
            continue;
        
        processedID.insHead(s.agentID);
        Monitor* m = lookFor(s.agentID);
        if(m == NULL)
        {
            m = addMonitor(s.agentID, s.q, s.p);
            /* predict possible states for next step */
	    /* For platoon */
	    Iterator<Sensing> is(sList);
	    Sensing tmpS;
	    List<State> qList;
	    while(is(tmpS))
	      qList.insHead(tmpS.q);

            m->predictStates(qList); //!!! FIXME: this is not the list of the target neighbors states
        }
        else
        {
            /* detect target maneuver */
            m->detectManeuver(s.q);
            if(m->isLocked())
            {
                /* building state list */
                Iterator<Sensing> is(sList);
                Sensing tmpS;
                List<State> qList;
                while(is(tmpS))
                    if(tmpS != s)
                        qList.insHead(tmpS.q);
                /* insert monitor agent state in the list */
                qList.insHead(agentQ);
                /* predict possible maneuvers */
                m->predictManeuvers(qList, obs);
            }
            /* predict possible states for next step */

	    /* For platoon */
	    Iterator<Sensing> is(sList);
	    Sensing tmpS;
	    List<State> qList;
	    while(is(tmpS))
	      if(tmpS != s)
		qList.insHead(tmpS.q);
	    
            m->predictStates(/* For platoon */ qList); //!!! FIXME: this is not the list of the target neighbors states
        }
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
    if(hList.count() > 0)
        error("MonitorLayer", "Hypothesis list MUST be empty");
    
    Iterator<Monitor*> i(monitorList);
    Monitor* m;
    while(i(m))
    {
        Hypothesis hyp;
        if(m->getFirstHypothesis(hyp))
            hList.insHead(hyp);
    }
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
        if(m->buildNeighborhood(n))
            nList.insHead(n);
    }
}
