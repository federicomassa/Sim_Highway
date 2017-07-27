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
#include "utility.h"
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

  std::cout << agentID << " receiving from " << k.getTargetID() << std::endl;

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
            std::cout << "Found new vehicle" << std::endl;
            Vector<List<Tensor5<bool> >, N_MANEUVER> compatibleHypotheses;
            Vector<List<Tensor5<Area> >, N_MANEUVER> hiddenCells;
            compareVehicleWithHidden(*newVehicle, myTmpNeigh.getTargetID(), compatibleHypotheses, hiddenCells, myTmpNeigh.getMappingArea());

            check(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID()));
            removeIncompatibleHiddenHypotheses(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft(), compatibleHypotheses, myTmpNeigh.getTargetID());
            removeIncompatibleFreeSpace(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft(),
                                        myTmpNeigh.getMappingArea(), k.obs, hiddenCells, myTmpNeigh.getTargetID());

            auxNeigh.getManeuversLeft() = Predictor::projectToManeuver(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft());
          }
          else
          {
            // remove incompatible free space using communicated visibility
            Vector<List<Tensor5<Area> >, N_MANEUVER> hiddenCells;

            Monitor* m = getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID());

            if (m)
            {
              const Vector<List<Tensor5<Sensing> >, N_MANEUVER>* hiddenState = nullptr;

              m->getHidden(hiddenState);

              if (hiddenState)
                getHiddenCells(*hiddenState, hiddenCells, myTmpNeigh.getMappingArea());
              else
                error("ReputationManager::singleMerge", "Empty hidden pointer");


              removeIncompatibleFreeSpace(getVehicle()->getMonitorLayer()->lookFor(myTmpNeigh.getTargetID())->getHypothesesLeft(),
                                          myTmpNeigh.getMappingArea(), k.obs, hiddenCells, myTmpNeigh.getTargetID());
            }
            else
              error("ReputationManager::singleMerge", "Empty monitor?");

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

void ReputationManager::compareVehicleWithHidden(const Sensing & newVehicle, const int& targetID, Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses, Vector<List<Tensor5<Area> >, N_MANEUVER>& hiddenCells, const Area & myMappingArea)
{
  Monitor* m = getVehicle()->getMonitorLayer()->lookFor(targetID, true);

  if (m)
  {
    const Vector<List<Tensor5<Sensing> >, N_MANEUVER>* hiddenState = nullptr;
    if (m)
      m->getHidden(hiddenState);
    else
      error("ReputationManager::compareVehicleWithHidden", "Predictor object not found");

    if (hiddenState)
      compatibleHypotheses = findCompatibleHypotheses(*hiddenState, newVehicle, hiddenCells, myMappingArea);
    else
      error("ReputationManager::compareVehicleWithHidden", "HiddenState object not found");
  }
  else
  {
    error("ReputationManager::compareVehicleWithHidden", "Monitor not found");
  }
}

Vector<List<Tensor5<bool> >, N_MANEUVER> ReputationManager::findCompatibleHypotheses(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& hidden, const Sensing & newVehicle, Vector<List<Tensor5<Area> >, N_MANEUVER>& hiddenCells, const Area & myMappingArea)
{
  Vector<List<Tensor5<bool> >, N_MANEUVER> compatibleHypotheses;
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    hiddenCells[sigma].purge();

    const Tensor5<Sensing>* hiddenTens;
    Tensor5<bool>* tens;

    List<Rectangle> rectList;
    myMappingArea.getRectList(rectList);
     // add dummy rectangle to replicate noHiddenVehicle situation
    rectList.insHead(Rectangle());
    
    const Rectangle* rect = nullptr;
    for (int list = 0; list < hidden[sigma].count(); list++)
    {
      // in hidden vector, the first element of the list corresponds to the no hidden vehicle hypothesis
      hidden[sigma].getElem(hiddenTens, list);
      rectList.getElem(rect, list);
      Matrix_2x2 bounds;
      rect->getBounds(bounds);

      compatibleHypotheses[sigma].insTail(Tensor5<bool>(hiddenTens->Dim1, hiddenTens->Dim2, hiddenTens->Dim3, hiddenTens->Dim4, hiddenTens->Dim5));
      compatibleHypotheses[sigma].getElem(tens, list);
      hiddenCells[sigma].insTail(Tensor5<Area>(tens->Dim1, tens->Dim2, tens->Dim3, tens->Dim4, tens->Dim5));
      Tensor5<Area>* hiddenCellTens;
      hiddenCells[sigma].getElem(hiddenCellTens, list);

      for (int i = 0; i < tens->Dim1; i++)
        for (int j = 0; j < tens->Dim2; j++)
          for (int k = 0; k < tens->Dim3; k++)
            for (int l = 0; l < tens->Dim4; l++)
              for (int m = 0; m < tens->Dim5; m++)
              {
                (*tens)(i, j, k, l, m) = false;

                // Area of competence of this particular hypothesis (grid cell). If list == 0 noHiddenVehicle --> null area because Dim1 = Dim2 = ... = 1
                Vector<Vector<double, 2>, 2> thisGridRectV;

                if (!rect->isDummy)
                {
                  thisGridRectV[0][0] = bounds[0][0] + Predictor::getDeltaX() * i;
                  thisGridRectV[0][1] = thisGridRectV[0][0] + Predictor::getDeltaX();
                  if (thisGridRectV[0][1] > bounds[0][1])
                    thisGridRectV[0][1] = bounds[0][1];

                  thisGridRectV[1][0] = bounds[1][0] + Predictor::getDeltaY() * j;
                  thisGridRectV[1][1] = thisGridRectV[1][0] + Predictor::getDeltaY();
                  if (thisGridRectV[1][1] > bounds[1][1])
                    thisGridRectV[1][1] = bounds[1][1];
                }
                else
                {
                  thisGridRectV[0][0] = 0.0;
                  thisGridRectV[0][1] = 0.0;
                  thisGridRectV[1][0] = 0.0;
                  thisGridRectV[1][1] = 0.0;
                }

                (*hiddenCellTens)(i, j, k, l, m).addRect(thisGridRectV);

                Vector<Vector<double, 2>, 2> mySensingRectV;
                mySensingRectV[0][0] = newVehicle.q.x - 3 * SIGMA_X;
                mySensingRectV[0][1] = newVehicle.q.x + 3 * SIGMA_X;
                mySensingRectV[1][0] = newVehicle.q.y - 3 * SIGMA_Y;
                mySensingRectV[1][1] = newVehicle.q.y + 3 * SIGMA_Y;

                Area mySensingArea;
                mySensingArea.addRect(mySensingRectV);

                // in the noHiddenVehicle case the intersection should be empty (Dim1 = Dim2 = 1), thus the presence of a hidden vehicle is incompatible if in the mapping area
                Area intersection = mySensingArea * (*hiddenCellTens)(i, j, k, l, m);
                intersection.simplify();

                if (list != 0)
                {
                  if (!intersection.isEmpty())
                    (*tens)(i, j, k, l, m) = true;
                }
                else
                {
                  if (myMappingArea.contains(newVehicle.q))
                    (*tens)(i, j, k, l, m) = false;
                  else
                    (*tens)(i, j, k, l, m) = true;
                }


              }
    }


  }


  return compatibleHypotheses;
}


void ReputationManager::getHiddenCells(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& hidden, Vector<List<Tensor5<Area> >, N_MANEUVER>& hiddenCells, const Area & myMappingArea)
{
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    hiddenCells[sigma].purge();

    const Tensor5<Sensing>* hiddenTens;

    List<Rectangle> rectList;
    myMappingArea.getRectList(rectList);
    // add dummy rectangle to replicate noHiddenVehicle situation
    rectList.insHead(Rectangle());

    const Rectangle* rect = nullptr;

    for (int list = 0; list < hidden[sigma].count(); list++)
    {
      // in hidden vector, the first element of the list corresponds to the no hidden vehicle hypothesis
      hidden[sigma].getElem(hiddenTens, list);
      hiddenCells[sigma].insTail(Tensor5<Area>(hiddenTens->Dim1, hiddenTens->Dim2, hiddenTens->Dim3, hiddenTens->Dim4, hiddenTens->Dim5));
      Tensor5<Area>* hiddenCellTens;
      hiddenCells[sigma].getElem(hiddenCellTens, list);

      rectList.getElem(rect, list);
      Matrix_2x2 bounds;
      rect->getBounds(bounds);

      for (int i = 0; i < hiddenTens->Dim1; i++)
        for (int j = 0; j < hiddenTens->Dim2; j++)
          for (int k = 0; k < hiddenTens->Dim3; k++)
            for (int l = 0; l < hiddenTens->Dim4; l++)
              for (int m = 0; m < hiddenTens->Dim5; m++)
              {

                // Area of competence of this particular hypothesis (grid cell). If list == 0 noHiddenVehicle --> null area because Dim1 = Dim2 = ... = 1
                Vector<Vector<double, 2>, 2> thisGridRectV;

                if (!rect->isDummy)
                {
                  thisGridRectV[0][0] = bounds[0][0] + Predictor::getDeltaX() * i;
                  thisGridRectV[0][1] = thisGridRectV[0][0] + Predictor::getDeltaX();
                  if (thisGridRectV[0][1] > bounds[0][1])
                    thisGridRectV[0][1] = bounds[0][1];

                  thisGridRectV[1][0] = bounds[1][0] + Predictor::getDeltaY() * j;
                  thisGridRectV[1][1] = thisGridRectV[1][0] + Predictor::getDeltaY();
                  if (thisGridRectV[1][1] > bounds[1][1])
                    thisGridRectV[1][1] = bounds[1][1];
                }
                else
                {
                  thisGridRectV[0][0] = 0.0;
                  thisGridRectV[0][1] = 0.0;
                  thisGridRectV[1][0] = 0.0;
                  thisGridRectV[1][1] = 0.0;
                }

                (*hiddenCellTens)(i, j, k, l, m).addRect(thisGridRectV);

              }
    }


  }

}

void ReputationManager::removeIncompatibleHiddenHypotheses(Vector<List<Tensor5<bool> >, N_MANEUVER>& hypothesesLeft, const Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses, const int& targetID)
{
  std::cout << "in removeIncompatibleHypotheses" << std::endl;
  Monitor* m = getVehicle()->getMonitorLayer()->lookFor(targetID);

  const Vector<List<Tensor5<Sensing> >, N_MANEUVER>* hiddenState = NULL;
  if (m)
  {
    m->getHidden(hiddenState);
  }

  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    Tensor5<bool>* leftTens;
    const Tensor5<bool>* compatibleTens;

    if (hypothesesLeft[sigma].count() != compatibleHypotheses[sigma].count())
      error("ReputationManager::removeIncompatibleHiddenHypotheses", std::string("Incompatible size lists: ") + toString(hypothesesLeft[sigma].count()) + " vs "  + toString(compatibleHypotheses[sigma].count()));

    for (int list = 0; list < hypothesesLeft[sigma].count(); list++)
    {
      hypothesesLeft[sigma].getElem(leftTens, list);
      const Tensor5<Sensing>* hiddenTens;
      (*hiddenState)[sigma].getElem(hiddenTens, list);
      compatibleHypotheses[sigma].getElem(compatibleTens, list);

      if (leftTens->Dim1 != compatibleTens->Dim1 ||
          leftTens->Dim2 != compatibleTens->Dim2 ||
          leftTens->Dim3 != compatibleTens->Dim3 ||
          leftTens->Dim4 != compatibleTens->Dim4 ||
          leftTens->Dim5 != compatibleTens->Dim5)
        error("ReputationManager::removeIncompatibleHiddenHypotheses", "Incompatible tensors");

      for (int i = 0; i < leftTens->Dim1; i++)
        for (int j = 0; j < leftTens->Dim2; j++)
          for (int k = 0; k < leftTens->Dim3; k++)
            for (int l = 0; l < leftTens->Dim4; l++)
              for (int m = 0; m < leftTens->Dim5; m++)
              {
                // In order to be compatible with the new vehicle it had to be compatible with the measure at the
                // end of the prediction (leftTens) and it has to geometrically match the hidden vehicle (compatibleTens)
                if ((*leftTens)(i, j, k, l, m) && (*compatibleTens)(i, j, k, l, m))
                {
                  // std::cout << "ACCEPTED: " << (*hiddenTens)(i, j, k, l, m) << std::endl;
                }
                else if ((*leftTens)(i, j, k, l, m) && !(*compatibleTens)(i, j, k, l, m))
                {
                  // std::cout << "REJECTED: " << (*hiddenTens)(i, j, k, l, m) << std::endl;
                }

                //works also for list == 0
                (*leftTens)(i, j, k, l, m) = (*leftTens)(i, j, k, l, m) && (*compatibleTens)(i, j, k, l, m);

              }
    }
  }
}

void ReputationManager::removeIncompatibleFreeSpace(Vector<List<Tensor5<bool> >, N_MANEUVER>& hypothesesLeft, const Area & myMappingArea, const Area & otherObs, const Vector<List<Tensor5<Area> >, N_MANEUVER>& hiddenCells, const int& targetID)
{

  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
  {
    require(hypothesesLeft[sigma].count() == hiddenCells[sigma].count(), "ReputationManager::removeIncompatibleFreeSpace", "List size incompatible");

    const Tensor5<Area>* hiddenCellTens;
    Tensor5<bool>* hypTens;

    for (int list = 0; list < hiddenCells[sigma].count(); list++)
    {
      hiddenCells[sigma].getElem(hiddenCellTens, list);
      hypothesesLeft[sigma].getElem(hypTens, list);

      require(hiddenCellTens->Dim1 == hypTens->Dim1 &&
              hiddenCellTens->Dim2 == hypTens->Dim2 &&
              hiddenCellTens->Dim3 == hypTens->Dim3 &&
              hiddenCellTens->Dim4 == hypTens->Dim4 &&
              hiddenCellTens->Dim5 == hypTens->Dim5, "ReputationManager::removeIncompatibleFreeSpace", "Incompatible tensor size");

      for (int i = 0; i < hiddenCellTens->Dim1; i++)
        for (int j = 0; j < hiddenCellTens->Dim2; j++)
          for (int k = 0; k < hiddenCellTens->Dim3; k++)
            for (int l = 0; l < hiddenCellTens->Dim4; l++)
              for (int m = 0; m < hiddenCellTens->Dim5; m++)
              {
                Area subtraction = (*hiddenCellTens)(i, j, k, l, m) - otherObs;

                // If empty, it means that this hidden vehicle (if list == 0 there was not hidden vehicle) could have been seen but wasn't --> wrong hypothesis
                if (subtraction.isEmpty() && list != 0)
                {
                  (*hypTens)(i, j, k, l, m) = false;
                }
              }
    }
  }


}
