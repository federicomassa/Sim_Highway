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

Monitor::Monitor(int a, int t, const State& tQ, const Parms& tP)
{
    agentID = a;
    targetID = t;
    targetQ = tQ;
    automaton.init(tQ);
    for(int i = 0; i < N_MANEUVER; i++)
        possibleManeuvers[i] = true;
    targetParms = tP;
    targetManeuver = UNKNOWN;
    targetLocked = false;
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
        LOG.s << "Computing possible events for vehicle " << targetID;
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
    
    /* estimate events for target agent */
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
    for(int i = 0; i < N_MANEUVER; i++)
    {
        Transition& t = automaton.transitions[targetManeuver][i];
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
    if(!certainEventDetected)
        /* hypothesis construction */
        for(int i = 0; i < N_MANEUVER; i++)
        {
            possibleHypLists[i].purge();
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
                                    sh.subEventID = i;
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
                    }
            }
        }
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC);
        LOG.s << "Computing possible events for vehicle " << targetID;
        LOG.s << " END." << EndLine();
    }
}

void Monitor::detectManeuver(const State& q)
{
    /* reset hypothesis */
    hypothesisList.purge();
    for(int i = 0; i < N_MANEUVER; i++)
        if(possibleStates[i] == q)
        {
            if(CONF.debug)
                LOG.s << "Detected maneuver " << (Maneuver)i << EndLine();
            if(targetLocked)
            {
                /* now Neighborhood object can be build */
                hypothesisList = possibleHypLists[i];
                hypReady = true;
            }
            if(targetManeuver == UNKNOWN)
                targetManeuver = (Maneuver)i;
            else if((Maneuver)i != targetManeuver || !CONF.monitorsNeedLock)
            {
                if(CONF.debug && CONF.monitorsNeedLock)
                    LOG.s << "Transition detected (target locked)" << EndLine();
                /* transition detected */
                targetManeuver = (Maneuver)i;
                automaton.setManeuver(targetManeuver, q);
                /* got a good estimation of target xi */
                targetLocked = true;
            }
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
    n.qTarget = targetLastQ;
    n.qList = lastNeighbors;
    n.hList = hypothesisList;
    
    return true;
}
