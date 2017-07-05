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
#include "Grid.h"
#include "Logger.h"

Monitor::Monitor(int a, int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<State>& qList) : predictor(a, t, CONF.nTimeSteps)
{
  agentID = a;
  targetID = t;
  targetQ = tQ;
  automaton.init(tQ);
  for (int i = 0; i < N_MANEUVER; i++)
    possibleManeuvers[i] = true;
  targetParms = tP;
  targetManeuver = UNKNOWN;
  realInitialManeuver = UNKNOWN;
  realFinalManeuver = UNKNOWN;
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
  while (i(tmpQ))
    if (active.contains(tmpQ.toPoint()))
      activeList.insHead(tmpQ);

  neighbors = activeList;

  /* It doesn't start right away, it starts at the next iteration
   so that it can receive a waiting request from other vehicles */
  timeStepsCount = -2;

  hypReady = false;
}

void Monitor::predictStates(const List<Sensing>& sList, const State& agentQ, const Maneuver& agentManeuver)
{


  monitorLog.s << "Real initial Monitor maneuver: " << realInitialManeuver << EndLine();

  monitorLog.s << "Prediction started..." << EndLine();

  /* reset value */
  hypReady = false;

  predictor.init(agentQ, agentManeuver, sList);

  predictor.run();
  predictor.getMonitor(monitorPrediction);

  if (CONF.savePredictionImages)
  {
    Grid g;
    for (int i = 0; i < N_MANEUVER; i++)
      for (int v1 = 0; v1 < 4; v1++)
        for (int v2 = v1 + 1; v2 < 4; v2++)
        {
          g.drawStateSpace(monitorPrediction[i], v1, v2);
          // C**time** A**agent** T**target** M**maneuver** V**pair of variables**
          g.save("Pred", "A" + toString(agentID, 2) + "T" + toString(targetID, 2) + "M" + toString(i) + "V" + toString(v1) + toString(v2));
        }
  }

}

void Monitor::detectManeuver(const State& agentQ, const State& q)
{
  hypothesisList.purge();

  targetLastManeuver = targetManeuver;

  lastPredictionAgentQ = predictionAgentQ;
  predictionAgentQ = agentQ;

  /* NB now detect maneuver is not called at each simulation step */
  targetLastQ = targetQ;
  targetQ = q;


  monitorLog.s << "Real final maneuver: " << realFinalManeuver << EndLine();

  targetManeuver = UNKNOWN;

  Vector<List<Tensor5<bool> >, N_MANEUVER> hypothesesLeft;

  Sensing monitorS(targetID, targetQ, targetQ.v, FAST);
  //std::cout << "Monitor measure: " << monitorS << std::endl;

  predictor.getErrors(errors, monitorS);


  if (Predictor::detectManeuver(monitorS, monitorPrediction, errors, hypothesesLeft))
  {
    List<Maneuver> maneuversLeft = Predictor::projectToManeuver(hypothesesLeft);
    monitorLog.s << "Detected Maneuvers: " << EndLine();

    for (int i = 0; i < maneuversLeft.count(); i++)
    {
      const Maneuver* man;
      maneuversLeft.getElem(man, i);

      /* FIXME ONLY FOR TESTING PURPOSES */
      if (i == 0)
        targetManeuver = *man;

      monitorLog.s << '\t' << *man << EndLine();
    }

    hypReady = true;
  }
  else
  {
    monitorLog.s << "Maneuver not detected: " << EndLine();
    hypReady = false;
  }



  /* Immediatly predict?
     timeStepsCount = 0;
     predictStates(...)
   */

  // FIXME!!!!!
  if (targetManeuver == UNKNOWN)
    targetManeuver = FAST;

  if (hypReady)
  {
    /* now Neighborhood object can be build */


    // hypothesisList should be dummy by now. Only using complete possibleHypLists
    hypothesisList = possibleHypLists[(int)targetManeuver];
  }

  automaton.setManeuver(targetManeuver, targetQ);

  if (targetManeuver != targetLastManeuver)
  {
    if (CONF.debug)
      LOG.s << "Transition detected" << EndLine();
  }

  hypothesisList.sort();

  lastHypLists = possibleHypLists;
  lastNeighbors = neighbors;

  /*  Iterator<State> iter(lastNeighbors);
  State tmpState;

  while (iter(tmpState))
    {
      if (floor(tmpState.y) == 1)
  {
    std::cout << "Neighbors state: " << tmpState.x << std::endl;
    break;
  }

    };

  std::cout << "lastPred state: " << lastPredictionAgentQ.x << std::endl;
  */

}

void Monitor::predictManeuvers(const List<State>& qList, const Area& obs)
{
  /* error handling */
  if (!hypReady)
    error("Monitor::predictManeuvers", "predictManeuvers called with monitor not ready");

  monitorLog.s << "Formulating hypotheses..." << EndLine();

  if (CONF.debug)
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
  while (i(tmpQ))
    if (active.contains(tmpQ.toPoint()))
      activeList.insHead(tmpQ);

  /* updating neighbors state list */
  /*  lastNeighbors = neighbors;*/
  neighbors = activeList;


  /* estimate events for target agent - non omniscient */
  automaton.detectEvents(targetQ, activeList, false);

  for (int i = 0; i < N_MANEUVER; i++)
  {
    possibleManeuvers[i] = false;
    possibleHypLists[i].purge();
  }

  /* compute sub-events indicator areas */
  bool valueChanged = false;
  Vector<Area, N_SUB_EVENT> subEventArea;
  Vector<int, N_SUB_EVENT> areaSign(0);
  if (CONF.debug)
    LOG.s << "SubEvents visit:" << EndLine();


  for (int k = 0; k < N_SUB_EVENT; k++)
  {
    SubEvent& se = automaton.subEvents[k];
    if (CONF.debug)
    {
      LOG.s << se << EndLine(EndLine::INC);
      LOG.s << "Evaluation time: " << se.evalTime;
      LOG.s << EndLine() << "Value: " << se.value;
    }
    if (se.value.nonOmniscientValue == U && se.evalTime == now)
    {
      if (se.areaFunc)
      {
        /* compute sub-event indicator area */
        Area indicator;
        se.areaFunc(targetQ, indicator);
        /* subtract observable area */
        indicator -= obs;
        if (indicator.isEmpty())
        {
          if (se.mode == OR)
            /* there isn't any agent where there should be */
            se.value.nonOmniscientValue = F;
          else
            /* there isn't any agent where there shouldn't be */
            se.value.nonOmniscientValue = T;
          valueChanged = true;
          if (CONF.debug)
            LOG.s << EndLine() << "Value changed: " << se.value;
        }
        else
        {
          subEventArea[k] = indicator;
          if (se.mode == OR)
            areaSign[k] = 1;
          else
            areaSign[k] = -1;
        }
      }
    }
    if (CONF.debug)
      LOG.s << EndLine(EndLine::DEC);
  }
  /* sub-events re-evaluation */
  if (valueChanged)
  {
    /* at least one sub-event value changed */
    if (CONF.debug)
      LOG.s << "SubEvents second pass:" << EndLine();
    for (int i = 0; i < N_MANEUVER; i++)
      automaton.transitions[targetManeuver][i].reEvaluate();
  }
  /* check for certain events */
  bool certainEventDetected = false;
  // if all transitions are false, then vehicle should keep the same maneuver
  ExtBool sameManeuverDetected = F;
  for (int i = 0; i < N_MANEUVER; i++)
  {
    if (targetManeuver == (Maneuver)i)
      continue;

    Transition& t = automaton.transitions[targetManeuver][i];
    sameManeuverDetected = sameManeuverDetected || t.getValue().nonOmniscientValue;
    if (t.getValue().nonOmniscientValue == T)
    {
      possibleManeuvers[i] = true;
      Iterator<Event*> ei(t.eventList);
      Event* e;
      while (ei(e))
        if (e->getValue().nonOmniscientValue == T)
        {
          /* build Hypothesis without uncertainty */
          LOG.s << "Added sure hypothesis on transition: " << targetManeuver << " => " << (Maneuver)i << ", event " << e->idx << EndLine();

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
    LOG.s << "Added sure hypothesis on transition: " << targetManeuver << " => " << targetManeuver << EndLine();

    Hypothesis h;
    possibleHypLists[(int)targetManeuver].insHead(h);
    certainEventDetected = true;
  }

  if (!certainEventDetected)
    /* hypothesis construction */
    for (int i = 0; i < N_MANEUVER; i++)
    {

      possibleHypLists[i].purge();

      if (targetManeuver == (Maneuver)i)
      {
        if (sameManeuverDetected == U)
        {
          LOG.s << "Added hypothesis on transition: " << targetManeuver << " => " << targetManeuver << EndLine();

          Hypothesis h;
          h.isSameManeuverUncertain = true;
          possibleHypLists[i].insHead(h);
        }
        continue;
      }

      Transition& t = automaton.transitions[targetManeuver][i];
      if (t.getValue().nonOmniscientValue == U)
      {
        possibleManeuvers[i] = true;
        Iterator<Event*> ie(t.eventList);
        Event* e;
        while (ie(e))
          if (e->value.nonOmniscientValue == U)
          {
            Hypothesis h;
            h.eventID = e->idx;
            Iterator<SubEvent*> ise(e->subEventList);
            SubEvent* se;
            while (ise(se))
              if (se->value.nonOmniscientValue == U)
              {
                int j = se->idx;
                if (areaSign[j] == 1)
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

            LOG.s << ", event " << e->idx;
            LOG.s << EndLine();
          }
      }
    }
  if (CONF.debug)
  {
    LOG.s << EndLine(EndLine::DEC);
    LOG.s << "Computing possible maneuvers for vehicle " << targetID << " as seen by vehicle " << agentID;
    LOG.s << " END." << EndLine();
  }
}

void Monitor::preDetectManeuver(const State& q, const Maneuver& sigma)
{
  //do not call the first time
  /*  if (!targetLocked)
      {
      targetLocked = true;
      return;
      }*/

  /* reset hypothesis */

  /* If sigma was not detected do nothing */
  if (sigma == UNKNOWN)
    return;

  hypothesisList.purge();

  targetLastManeuver = targetManeuver;

  if (CONF.debug)
    LOG.s << "Detected maneuver " << sigma << EndLine();
  if (targetLocked)
  {
    /* now Neighborhood object can be build */
    hypothesisList = possibleHypLists[(int)sigma];
    hypReady = true;
  }

  automaton.setManeuver(sigma, q);

  if (sigma != targetManeuver || !CONF.monitorsNeedLock)
  {
    if (CONF.debug && CONF.monitorsNeedLock)
      LOG.s << "Transition detected (target locked)" << EndLine();
    /* transition detected */
    targetManeuver = sigma;

    /* got a good estimation of target xi */
    targetLocked = true;
  }

  hypothesisList.sort();
  targetLastQ = targetQ;
  targetQ = q;

  lastHypLists = possibleHypLists;

}

bool Monitor::getFirstHypothesis(Hypothesis& hyp) const
{
  return hypothesisList.getFirst(hyp);
}

bool Monitor::buildNeighborhood(Neighborhood& n) const
{
  if (!hypReady)
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


