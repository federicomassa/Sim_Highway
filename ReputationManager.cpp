/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        ReputationManager.cpp
 * DESCRIPTION: Implementation of the ReputationManager class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "ReputationManager.h"
#include "Vehicle.h"
#include "Tensor5.h"
#include <utility>

void ReputationManager::
merge(const List<Message<Knowledge> >& msgList)
{
  Iterator<Message<Knowledge> > mi(msgList);
  Message<Knowledge> tmpMsg;
  while (mi(tmpMsg))
  {
    Knowledge tmpK;
    tmpMsg.getBody(tmpK); /* body extraction */

    singleMerge(tmpK);
  }
}

void ReputationManager::init(Channel<Knowledge>* rC, int a, Vehicle* v)
{
  /* error handling */
  if (rC == NULL)
    error("ReputationManager", "pointer to the channel is NULL");
  if (a < 0)
    error("ReputationManager", "NEGATIVE agent identifier");

  /* initialization */
  repChan = rC;
  agentID = a;
  vehicle = v;
}

void ReputationManager::setCurrentParams(const State& s,
    const Knowledge& k)
{
  if (CONF.debug)
    LOG.s << "Setting current parameters" << EndLine(EndLine::INC);

  q = s;
  knowledge = k;

  if (CONF.debug)
  {
    LOG.s << "Current state: " << q << EndLine();
    LOG.s << "Current knowledge: " << knowledge.nList << EndLine(EndLine::DEC);
  }
}

void ReputationManager::singleMerge(const Knowledge& k)
{


  /*
  std::cout << "My neighborhood list empty? " << knowledge.nList.isEmpty() << std::endl;
  std::cout << "My mVehicles list empty? " << knowledge.monitoredVehicles.isEmpty() << std::endl;

  std::cout << "Recv neighborhood list empty? " << k.nList.isEmpty() << std::endl;
  std::cout << "Recv mVehicles list empty? " << k.monitoredVehicles.isEmpty() << std::endl;*/

  List<Neighborhood> nL = k.nList;

  if (!nL.isEmpty())
  {

    Iterator<Neighborhood> ni(nL);
    Neighborhood tmpNeigh;
    while (ni(tmpNeigh))
    {
      Iterator<Neighborhood> myni(knowledge.nList);
      Neighborhood myTmpNeigh;
      bool targetFound = false;
      while (myni(myTmpNeigh))
        if (myTmpNeigh == tmpNeigh)
        {
          Neighborhood auxNeigh = myTmpNeigh; /* working on a copy */
          Sensing* newVehicle = 0;
          auxNeigh.intersectionWith(tmpNeigh, newVehicle); /* merging */

          if (newVehicle)
          {
            Vector<List<Tensor5<bool> >, N_MANEUVER> compatibleHypotheses;
            compareVehicleWithHidden(*newVehicle, myTmpNeigh.getTargetID(), compatibleHypotheses);

            check(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID()));
            removeIncompatibleHypotheses(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft(), compatibleHypotheses);

            auxNeigh.getManeuversLeft() = Predictor::projectToManeuver(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft());
          }

          knowledge.nList.updateInfo(myTmpNeigh, auxNeigh); /* updating */
          /* target can be found one single time in a list: stop */
          targetFound = true;

          if (newVehicle)
            delete newVehicle;

          break;
        }
      // if(!targetFound) //FIXME... solo nel merge: treat the case "agent too far"
      //   nList.insHead(tmpNeigh); /* insert a ``new'' target */
    }
  }
  /* nList is empty, maybe the vehicle was asked to wait for prediction? */
  else if (!k.monitoredVehicles.isEmpty())
  {
    Iterator<std::pair<int, int> > kI(k.monitoredVehicles);
    std::pair<int, int> p, myP;

    while (kI(p))
    {
      Iterator<std::pair<int, int> > myI(knowledge.monitoredVehicles);
      while (myI(myP))
      {
        /* same target */
        if (p.first == myP.first)
        {
          /* If you are idle */
          if (myP.second < 0)
          {
            int waitRequest;

            /* Compute waiting time requested by other vehicle */
            if (p.second >= 0)
              waitRequest = CONF.nTimeSteps - p.second + 1;
            else
              waitRequest = -p.second;

            /* If someone is asking you to wait longer than you would otherwise, change your countdown
             to sync with the other vehicles. */
            if (waitRequest > (-myP.second))
            {
              knowledge.monitoredVehicles.updateInfo(myP, std::make_pair(myP.first, -waitRequest));
            }
          }

          break;
        }
      };
    };

  }
}

void ReputationManager::getAgentsReputation(List<Reputation>& repList) const
{
  Iterator<Neighborhood> myni(knowledge.nList);
  Neighborhood myTmpNeigh;

  while (myni(myTmpNeigh))
  {
    Reputation r;
    r.targetID = myTmpNeigh.getTargetID();
    r.qTarget = myTmpNeigh.getTargetState();
    r.level.insert(myTmpNeigh.getTargetReputation().begin(), myTmpNeigh.getTargetReputation().end());
    repList.insHead(r);
  }
}

void ReputationManager::compareVehicleWithHidden(const Sensing& newVehicle, const int& targetID, Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses)
{
  Monitor* m = getVehicle()->getMonitorLayer()->lookFor(targetID);
  if (m)
  {
    const Predictor* p = m->getPredictor();
    const Vector<List<Tensor5<Sensing> >, N_MANEUVER>* hiddenState = NULL;
    if (p)
      p->getHidden(hiddenState);
    else
      error("ReputationManager::checkNewVehicle", "Predictor object not found");

    if (hiddenState)
      compatibleHypotheses = findCompatibleHypotheses(*hiddenState, newVehicle);
    else
      error("ReputationManager::checkNewVehicle", "HiddenState object not found");
  }
  else
  {
    error("ReputationManager::checkNewVehicle", "Monitor not found");
  }
}

Vector<List<Tensor5<bool> >, N_MANEUVER> ReputationManager::findCompatibleHypotheses(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& hidden, const Sensing& newVehicle)
{
  Vector<List<Tensor5<bool> >, N_MANEUVER> compatibleHypotheses;
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    const Tensor5<Sensing>* hiddenTens;
    Tensor5<bool>* tens;
    for (int list = 0; list < hidden[sigma].count(); list++)
    {
      hidden[sigma].getElem(hiddenTens, list);
      compatibleHypotheses[sigma].insTail(Tensor5<bool>(hiddenTens->Dim1, hiddenTens->Dim2, hiddenTens->Dim3, hiddenTens->Dim4, hiddenTens->Dim5));
      compatibleHypotheses[sigma].getElem(tens, list);

      for (int i = 0; i < tens->Dim1; i++)
        for (int j = 0; j < tens->Dim2; j++)
          for (int k = 0; k < tens->Dim3; k++)
            for (int l = 0; l < tens->Dim4; l++)
              for (int m = 0; m < tens->Dim5; m++)
              {
                (*tens)(i, j, k, l, m) = false;

                // Area of competence of this particular hypothesis (grid cell)
                Vector<Vector<double, 2>, 2> thisGridRectV;

                if (i == 0)
                  thisGridRectV[0][0] = (*hiddenTens)(i, j, k, l, m).q.x;
                else
                  thisGridRectV[0][0] = ((*hiddenTens)(i, j, k, l, m).q.x + (*hiddenTens)(i - 1, j, k, l, m).q.x) / 2.0;

                if (i == tens->Dim1 - 1)
                  thisGridRectV[0][1] = (*hiddenTens)(i, j, k, l, m).q.x;
                else
                  thisGridRectV[0][1] = ((*hiddenTens)(i, j, k, l, m).q.x + (*hiddenTens)(i + 1, j, k, l, m).q.x) / 2.0;

                if (j == 0)
                  thisGridRectV[1][0] = (*hiddenTens)(i, j, k, l, m).q.y;
                else
                  thisGridRectV[1][0] = ((*hiddenTens)(i, j, k, l, m).q.y + (*hiddenTens)(i, j - 1, k, l, m).q.y) / 2.0;

                if (j == tens->Dim2 - 1)
                  thisGridRectV[1][1] = (*hiddenTens)(i, j, k, l, m).q.y;
                else
                  thisGridRectV[1][1] = ((*hiddenTens)(i, j, k, l, m).q.y + (*hiddenTens)(i, j + 1, k, l, m).q.y) / 2.0;

                Area thisGridCell;
                thisGridCell.addRect(thisGridRectV);

                Vector<Vector<double, 2>, 2> mySensingRectV;
                mySensingRectV[0][0] = newVehicle.q.x - 3 * SIGMA_X;
                mySensingRectV[0][1] = newVehicle.q.x + 3 * SIGMA_X;
                mySensingRectV[1][0] = newVehicle.q.y - 3 * SIGMA_Y;
                mySensingRectV[1][1] = newVehicle.q.y + 3 * SIGMA_Y;

                Area mySensingArea;
                mySensingArea.addRect(mySensingRectV);

                Area intersection = mySensingArea * thisGridCell;
                intersection.simplify();

                if (!intersection.isEmpty())
                  (*tens)(i, j, k, l, m) = true;


              }
    }


  }


  return compatibleHypotheses;
}

void ReputationManager::removeIncompatibleHypotheses(Vector<List<Tensor5<bool> >, N_MANEUVER>& hypothesesLeft, const Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses)
{
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    Tensor5<bool>* leftTens;
    const Tensor5<bool>* compatibleTens;

    if (hypothesesLeft[sigma].count() != compatibleHypotheses[sigma].count())
      error("ReputationManager::removeIncompatibleHypotheses", "Incompatible size lists");

    for (int list = 0; list < hypothesesLeft[sigma].count(); list++)
    {
      hypothesesLeft[sigma].getElem(leftTens, list);
      compatibleHypotheses[sigma].getElem(compatibleTens, list);

      if (leftTens->Dim1 != compatibleTens->Dim1 ||
          leftTens->Dim2 != compatibleTens->Dim2 ||
          leftTens->Dim3 != compatibleTens->Dim3 ||
          leftTens->Dim4 != compatibleTens->Dim4 ||
          leftTens->Dim5 != compatibleTens->Dim5)
        error("ReputationManager::removeIncompatibleHypotheses", "Incompatible tensors");

      for (int i = 0; i < leftTens->Dim1; i++)
        for (int j = 0; j < leftTens->Dim2; j++)
          for (int k = 0; k < leftTens->Dim3; k++)
            for (int l = 0; l < leftTens->Dim4; l++)
              for (int m = 0; m < leftTens->Dim5; m++)
              {
                // In order to be compatible with the new vehicle it had to be compatible with the measure at the 
                // end of the prediction (leftTens) and it has to geometrically match the hidden vehicle (compatibleTens)
                (*leftTens)(i,j,k,l,m) = (*leftTens)(i,j,k,l,m) && (*compatibleTens)(i,j,k,l,m);
              }
    }
  }
}