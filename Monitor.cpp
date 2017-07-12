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

Monitor::Monitor(int a, int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<Sensing>& sList) : predictor(a, t, CONF.nTimeSteps)
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
  automaton.setManeuver(tSigma, tQ);
  //build starting neighborhood
  /* compute active area for target agent */
  Area active;
  activeArea(tQ, active);

  /* determine list of ``active'' agents */
  Iterator<Sensing> i(sList);
  Sensing tmpS;
  List<Sensing> activeList;
  while (i(tmpS))
    if (active.contains(tmpS.q.toPoint()))
      activeList.insHead(tmpS);

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
  lastManeuversLeft = maneuversLeft;

  lastPredictionAgentQ = predictionAgentQ;
  predictionAgentQ = agentQ;

  /* NB now detect maneuver is not called at each simulation step */
  targetLastQ = targetQ;
  targetQ = q;


  monitorLog.s << "Real final maneuver: " << realFinalManeuver << EndLine();

  targetManeuver = UNKNOWN;

  Sensing monitorS(targetID, targetQ, targetQ.v, FAST);
  //std::cout << "Monitor measure: " << monitorS << std::endl;

  predictor.getErrors(errors, monitorS);


  if (predictor.detectManeuver(monitorS, monitorPrediction, errors, hypothesesLeft))
  {
    maneuversLeft = Predictor::projectToManeuver(hypothesesLeft);
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
    //hypothesisList = possibleHypLists[(int)targetManeuver];
  }

  //automaton.setManeuver(targetManeuver, targetQ);

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

void Monitor::predictManeuvers(const State& tQ, const List<Sensing>& sList, const Area& obs, const Area& monitorObs)
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

  targetQ = tQ;
  /* compute active area for target agent */
  Area active;
  activeArea(targetQ, active);

  /* determine list of ``active'' agents */
  Iterator<Sensing> i(sList);
  Sensing tmpS;
  List<Sensing> activeList;
  List<State> activeListQ;

  while (i(tmpS))
    if (active.contains(tmpS.q.toPoint()))
    {
      activeList.insHead(tmpS);
      activeListQ.insHead(tmpS.q);
    }

  if (agentID == 0 && targetID == 1)
  {
    std::cout << "Monitor sees " << activeListQ << std::endl;
    std::cout << "Monitor obs " << monitorObs << std::endl;
  }

  /* updating neighbors state list */
  /*  lastNeighbors = neighbors;*/
  neighbors = activeList;



  /* Purge and re-init previously filled lists */
  possibleHypLists.clear();
  for (int manLeftCount = 0; manLeftCount < maneuversLeft.count(); manLeftCount++)
  {
    const Maneuver* man;
    maneuversLeft.getElem(man, manLeftCount);

    // map::operator[] creates key if non-existent
    possibleHypLists[*man];

    // First evaluation of events - without considering visible areas
    automaton.setManeuver(*man, targetQ);
    automaton.detectEvents(targetQ, activeListQ, false);
  }

  //DEBUG
  if (agentID == 0 && targetID == 1)
  {
    List<Event*> eL = automaton.transitions[FAST][SLOW].eventList;
    Event* suspectEvent = NULL;
    Event* e;
    Iterator<Event*> eItr(eL);

    while (eItr(e))
    {
      if (e->getID() == 0)
      {
        suspectEvent = e;
        break;
      }
    }

    if (suspectEvent == NULL)
      error("WTF", "WTF");

    List<SubEvent*> seL = suspectEvent->subEventList;
    Iterator<SubEvent*> seItr(seL);
    SubEvent* suspectSubEvent = NULL;
    SubEvent* se;
    while (seItr(se))
    {
      if (se->getID() == 19)
      {
        suspectSubEvent = se;
        break;
      }
    }

    if (suspectSubEvent == NULL)
      error("WTFSUB", "WTFSUB");

    std::cout << "Found in automaton: " << suspectSubEvent->value << std::endl;

  }


  for (int i = 0; i < N_MANEUVER; i++)
  {
    possibleManeuvers[i] = false;
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

        if (agentID == 0 && targetID == 1 && se.getID() == 19)
        {
          std::cout << "Before cross " << indicator << std::endl;
        }
        // Addition: we do not care if there are doubts in areas that the monitor cannot see, right?
        indicator *= monitorObs;
        indicator.simplify();

        if (agentID == 0 && targetID == 1 && se.getID() == 19)
        {
          std::cout << "After cross " << indicator << std::endl;
        }


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
      for (int j = 0; j < N_MANEUVER; j++)
        automaton.transitions[i][j].reEvaluate();
  }


  for (auto hypItr = possibleHypLists.begin(); hypItr != possibleHypLists.end(); hypItr++)
  {
    /* check for certain events */
    bool certainEventDetected = false;
    // if all transitions are false, then vehicle should keep the same maneuver
    ExtBool sameManeuverDetected = F;

    targetManeuver = (*hypItr).first;

    Vector<List<Hypothesis>, N_MANEUVER>& possibleHyp = (*hypItr).second;

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
            possibleHyp[i].insHead(h);
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
      possibleHyp[(int)targetManeuver].insHead(h);
      certainEventDetected = true;
    }

    if (!certainEventDetected)
      /* hypothesis construction */
      for (int i = 0; i < N_MANEUVER; i++)
      {

        possibleHyp[i].purge();

        if (targetManeuver == (Maneuver)i)
        {
          if (sameManeuverDetected == U)
          {
            LOG.s << "Added hypothesis on transition: " << targetManeuver << " => " << targetManeuver << EndLine();

            Hypothesis h;
            h.isSameManeuverUncertain = true;
            possibleHyp[i].insHead(h);
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
                    if (j == 19 && agentID == 0 && targetID == 1 && targetManeuver == FAST && (Maneuver)i == SLOW)
                      std::cout << "Found in monitor: " << subEventArea[j] << std::endl;

                    h.negative += subEventArea[j];
                  }
                }
              /* add generated hypothesis */
              possibleHyp[i].insHead(h);
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
  }

  if (CONF.debug)
  {
    LOG.s << EndLine(EndLine::DEC);
    LOG.s << "Computing possible maneuvers for vehicle " << targetID << " as seen by vehicle " << agentID;
    LOG.s << " END." << EndLine();
  }
}

void Monitor::preDetectManeuver(const State & q, const Maneuver & sigma)
{
  //do not call the first time
  /*  if (!targetLocked)
      {
      targetLocked = true;
      return;
      }*/

  /* reset hypothesis */

  /* If sigma was not detected do nothing */

  /*
  if (sigma == UNKNOWN)
    return;

  hypothesisList.purge();

  targetLastManeuver = targetManeuver;

  if (CONF.debug)
    LOG.s << "Detected maneuver " << sigma << EndLine();
  if (targetLocked)
  {*/
  /* now Neighborhood object can be build */
  /*   hypothesisList = possibleHypLists[(int)sigma];
     hypReady = true;
   }

   automaton.setManeuver(sigma, q);

   if (sigma != targetManeuver || !CONF.monitorsNeedLock)
   {
     if (CONF.debug && CONF.monitorsNeedLock)
       LOG.s << "Transition detected (target locked)" << EndLine(); */
  /* transition detected */

  //  targetManeuver = sigma;

  /* got a good estimation of target xi */
  //  targetLocked = true;
// }
  /*
    hypothesisList.sort();
    targetLastQ = targetQ;
    targetQ = q;

    lastHypLists = possibleHypLists;
  */
}

bool Monitor::getFirstHypothesis(Hypothesis & hyp) const
{
  return hypothesisList.getFirst(hyp);
}

bool Monitor::buildNeighborhood(Neighborhood & n) const
{
  if (!hypReady)
    return false;

  n.targetID = targetID;
  n.agentID = agentID;
  n.qTarget = targetLastQ;
  n.sList = lastNeighbors;
  n.maneuversLeft = maneuversLeft;
  n.lastManeuversLeft = lastManeuversLeft;
  n.targetManeuver = targetManeuver;
  n.targetLastManeuver = targetLastManeuver;
  n.hList = hypothesisList;
  n.lastHypLists.clear();
  n.lastHypLists.insert(lastHypLists.begin(), lastHypLists.end());


  return true;
}


