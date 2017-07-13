
/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Neighborhood.cpp
 * DESCRIPTION: Implementation of the Neighborhood class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Neighborhood.h"
#include "Sensing.h"
#include "ExtValue.h"
#include <iostream>

using namespace std;

// Template specialization for sList merging
template<>
bool List<Sensing>::join(const List<Sensing>& l)
{
  /* chek if the list l is empty */
  if (l.head == NULL)
    return true;

  /* join */
  bool result = false;
  Elem* aux;
  for (aux = l.head; aux != NULL; aux = aux->next)
  {
    bool alreadyPresent = false;
    Elem* myAux;
    for (myAux = (*this).head; myAux != NULL; myAux = myAux->next)
    {
      if (aux->info.agentID == myAux->info.agentID)
      {
        alreadyPresent = true;
        break;
      }
    }

    if (!alreadyPresent)
    {
      bool tmp = insHead(aux->info, true);
      result = result || tmp;
    }
  }

  return result;
}

Neighborhood::Neighborhood(int t, const State& qT, const List<Sensing>& sL,
                           const List<Hypothesis>& hL, const map<Maneuver, Vector<List<Hypothesis>, N_MANEUVER> >& lastHL)
{
  targetID = t;
  qTarget = qT;

  Iterator<Sensing> is(sL);
  Sensing s;

  while (is(s))
  {
    sList.insHead(s);
  }


  hList = hL;
  lastHypLists.insert(lastHL.begin(), lastHL.end());
}

void Neighborhood::intersectionWith(const Neighborhood& n, Sensing*& foundNewVehicle)
{
  /* error handling */
  if (targetID != n.targetID)
    error("Neighborhood", "cannot intersect with different targetID");

  if (this == &n)
    return;

  // ========== hypList merging: probably USELESS, replaced by possibleHypLists ============
  Hypothesis hyp;
  int nHyp = hList.count();
  for (int i = 0; i < nHyp; i++)
  {
    hList.extrHead(hyp);
    /* get corresponding hypothesis */
    Hypothesis tmpHyp;
    if (n.hList.find(hyp, tmpHyp))
    {
      bool valid = true;
      /* intersection between negative areas */
      hyp.negative *= tmpHyp.negative;
      /* intersection between sub-hypothesis */
      List<Hypothesis::SubHypothesis>& shList = hyp.subHypList;
      Hypothesis::SubHypothesis sHyp;
      int nsHyp = shList.count();
      for (int j = 0; j < nsHyp; j++)
      {
        shList.extrHead(sHyp);
        Hypothesis::SubHypothesis tmpSHyp;
        if (tmpHyp.subHypList.find(sHyp, tmpSHyp))
        {
          sHyp.positive *= tmpSHyp.positive;
          if (sHyp.positive.isEmpty())
          {
            valid = false;
            break;
          }
          shList.insTail(sHyp);
        }
      }
      if (valid)
        hList.insTail(hyp);
    }
  }
  // ============== END OF hypList MERGING ======================

  // ========= Merging of possibleHypLists =============

  for (auto hypItr = lastHypLists.begin(); hypItr != lastHypLists.end(); hypItr++)
  {
    Vector<List<Hypothesis>, N_MANEUVER> ntmpHL_v;
    Vector<List<Hypothesis>, N_MANEUVER>& tmpHL_v = lastHypLists.at((*hypItr).first);

    try
    {
      ntmpHL_v = n.lastHypLists.at((*hypItr).first);
    }
    catch (std::out_of_range& e)
    {
      // other agent did not find this maneuver (absent in the maneuversLeft), do not merge that
      continue;
    }

    for (int i = 0; i < N_MANEUVER; i++)
    {
      List<Hypothesis>& tmpHL = tmpHL_v[i];
      List<Hypothesis> ntmpHL = ntmpHL_v[i];
      Hypothesis hyp;
      int numHyp = tmpHL.count();
      for (int n = 0; n < numHyp; n++)
      {
        tmpHL.extrHead(hyp);
        /* get corresponding hypothesis, excluding same maneuver case with -1 */
        Hypothesis tmpHyp;
        if (ntmpHL.find(hyp, tmpHyp) && hyp.eventID != -1)
        {
          bool valid = true;
          /* intersection between negative areas */
          hyp.negative *= tmpHyp.negative;
          /* intersection between sub-hypothesis */
          List<Hypothesis::SubHypothesis>& shList = hyp.subHypList;
          Hypothesis::SubHypothesis sHyp;
          int nsHyp = shList.count();
          for (int j = 0; j < nsHyp; j++)
          {
            shList.extrHead(sHyp);
            Hypothesis::SubHypothesis tmpSHyp;
            if (tmpHyp.subHypList.find(sHyp, tmpSHyp))
            {
              sHyp.positive *= tmpSHyp.positive;
              if (sHyp.positive.isEmpty())
              {
                valid = false;
                break;
              }
              shList.insTail(sHyp);
            }
          }
          if (valid)
          {
            // CHECK?? if the hyp was negative-only. In that case, if negative is now empty,
            // you can say something more about the reputation
            // if (hyp.subHypList.isEmpty() && negative.isEmpty())
            tmpHL.insTail(hyp);
          }
        }
        else if (hyp.eventID == -1 && ntmpHL.find(hyp, tmpHyp))
          tmpHL.insTail(hyp);



      }
      /* sort hypotesis list */
      tmpHL.sort();

    }
  }

  // ========= End of lastHypLists merging =========

  /* merging qList with n.qList */

  // Joining the sList is not trivial, because of sensor error measurements.
  // We could use the communication to improve the state estimation, but for now
  // we limit to ignore the vehicles with same ID --- see template specialization on top
  List<Sensing> preIntersectNeighbors = sList;
  sList.join(n.sList);
  List<Sensing> postIntersectNeighbors = sList;

  std::cout << agentID << " merging... " << preIntersectNeighbors.count() << " -> " << postIntersectNeighbors.count() << std::endl;

  if (postIntersectNeighbors.count() - preIntersectNeighbors.count() > 0)
  {
    if (postIntersectNeighbors.count() - preIntersectNeighbors.count() == 1)
    {
      // handle hypotheses merging
      Iterator<Sensing> postItr(postIntersectNeighbors);
      Sensing postS;

      while (postItr(postS))
      {
        Sensing preS;
        if (!preIntersectNeighbors.find(postS, preS))
        {
          std::cout << "Found new veh" << std::endl;
          foundNewVehicle = new Sensing(postS);
        }
      }
    }
    else
    {
      // more than one hidden agent case, what do we do?
      error("Neighborhood::intersectionWith", "Found more than one hidden agent. How do we handle this?");
    }
  }


  /*
  // Now look for pairs with same state and different maneuver: merging
  for (int i = 0; i < extQList.count(); i++)
    {
  for (int j = i+1; j < extQList.count(); j++)
  {
    if (extQList[i].first == extQList[j].first &&
  extQList[i].second != extQList[j].second)
      {
  if (extQList[i].second == UNKNOWN)
    extQList.delInfo(extQList[i]);
  else if (extQList[j].second == UNKNOWN)
    extQList.delInfo(extQList[j]);
      }
  }
    }
  */


}

// a reputation for each possible transition Maneuver1 -> Maneuver2
map<pair<Maneuver, Maneuver>, RepLevel> Neighborhood::getTargetReputation() const
{
  map<pair<Maneuver, Maneuver>, RepLevel> level;
  /* Old reputation system
    if(hList.isEmpty())
        return FAULTY;

    Iterator<Hypothesis> i(hList);
    Hypothesis hyp;
    while(i(hyp))
      if(!hyp.subHypList.isEmpty() || !hyp.negative.isEmpty() || hyp.isSameManeuverUncertain)
            return UNCERTAIN;

      return CORRECT; */

  // Count possible maneuvers
  Iterator<Maneuver> lastManLeftItr(lastManeuversLeft);
  Maneuver lastManLeft;

  while (lastManLeftItr(lastManLeft))
  {

    ExtBool possibleManeuver[N_MANEUVER];

    for (int i = 0;  i < N_MANEUVER; i++)
    {
      possibleManeuver[i] = F;

      List<Hypothesis> tmpHL;

      try
      {
        tmpHL = lastHypLists.at(lastManLeft)[i];
      }
      catch (std::out_of_range& e)
      {
        error("Neighborhood::getTargetReputation", "Last Hypothesis not found");
      }

      // No hypotheses -> stay false, next maneuver
      if (tmpHL.isEmpty())
        continue;

      possibleManeuver[i] = T;

      Iterator<Hypothesis> iHyp(tmpHL);
      Hypothesis tmpHyp;

      while (iHyp(tmpHyp))
      {
        /*
        //DEBUG
        if ((Maneuver)i == PLATOON && targetLastManeuver == FAST && targetManeuver == PLATOON)
          {
            std::cout << "sub: " << tmpHyp.subHypList.isEmpty() << std::endl;
            std::cout << "neg: " << tmpHyp.negative.isEmpty() << std::endl;
          }
        */

        possibleManeuver[i] = possibleManeuver[i] &&
                              (tmpHyp.subHypList.isEmpty() ? T : U) &&
                              (tmpHyp.negative.isEmpty() ? T : U);


        // FIXME If the order of the hypotheses is true - uncertain it breaks, probably correct beh, but
        // if the order is uncertain - true it is uncertain. I don't remember why I did it.
        if (possibleManeuver[i] == T)
          break;
      }
    }

    Iterator<Maneuver> manLeftItr(maneuversLeft);
    Maneuver manLeft;

    while (manLeftItr(manLeft))
    {
      RepLevel& currLevel = level[make_pair(lastManLeft, manLeft)];
      //std::cout << "Filling reputation..." << lastManLeft << " => " << manLeft << level.at(make_pair(lastManLeft, manLeft)) << std::endl;


      // Now calculate reputation
      //     Case 1: same maneuver
      if (manLeft == lastManLeft)
      {
        if (possibleManeuver[manLeft] == F)
        {
          currLevel = FAULTY;
          continue;
        }

        ExtBool isCorrect = T;
        for (int i = 0; i < N_MANEUVER; i++)
        {
          if (i == (int)manLeft)
            continue;

          isCorrect = isCorrect && !possibleManeuver[i];

        }

        if (isCorrect == T)
        {
          currLevel = CORRECT;
          continue;
        }
        // TEST ADDITION, see how it was before in git
        else if (isCorrect == U)
        {
          currLevel = UNCERTAIN;
          continue;
        }
        else if (isCorrect == F)
        {
          currLevel = FAULTY;
          continue;
        }

      }

      // Case 2: maneuver transition
      else
      {
        if (possibleManeuver[manLeft] == T)
        {
          currLevel = CORRECT;
          continue;
        }
        else if (possibleManeuver[manLeft] == U)
        {
          currLevel = UNCERTAIN;
          continue;
        }
        else if (possibleManeuver[manLeft] == F)
        {
          currLevel = FAULTY;
          continue;
        }
      }
    }
  }

  return level;

}

double Neighborhood::measure() const
{
  double mu = 0;
  Iterator<Hypothesis> i(hList);
  Hypothesis h;
  while (i(h))
    mu += h.measure();

  return mu;
}

ostream& operator<<(ostream& os, const Neighborhood& n)
{
  os << "Neighborhood {" << EndLine(EndLine::INC);
  os << "Target ID: " << n.targetID << EndLine();
  os << "Target's state: " << n.qTarget << EndLine();
  os << "State list: " << n.sList << EndLine();
  os << "Hypothesis list: " << n.hList << EndLine();
  os << "Measure (Mu): " << n.measure() << EndLine(EndLine::DEC);
  os << '}';

  return os;
}

/*
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Testing getTargetReputation function", "[reputation]")
{
  Neighborhood n;
  n.getLastManeuversLeft().insTail(FAST);
  n.getLastManeuversLeft().insTail(SLOW);

  n.getManeuversLeft().insTail(LEFT);
  n.getManeuversLeft().insTail(RIGHT);

  Hypothesis h;
  Hypothesis::SubHypothesis s;
  s.positive = Area();
  h.negative = Area();
  h.subHypList.insTail(s);

  List<Hypothesis> lh;
  lh.insTail(h);

  n.getLastHypotheses()[FAST][(int)LEFT] = lh;
  n.getLastHypotheses()[SLOW][(int)LEFT] = lh;

  std::map<std::pair<Maneuver, Maneuver>, RepLevel> level = n.getTargetReputation();
  REQUIRE(level.at(make_pair(FAST, RIGHT)) == FAULTY);
  REQUIRE(level.at(make_pair(SLOW, RIGHT)) == FAULTY);

}
*/
