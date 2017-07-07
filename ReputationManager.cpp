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

void ReputationManager::init(Channel<Knowledge>* rC, int a)
{
  /* error handling */
  if (rC == NULL)
    error("ReputationManager", "pointer to the channel is NULL");
  if (a < 0)
    error("ReputationManager", "NEGATIVE agent identifier");

  /* initialization */
  repChan = rC;
  agentID = a;
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
          auxNeigh.intersectionWith(tmpNeigh); /* merging */
          knowledge.nList.updateInfo(myTmpNeigh, auxNeigh); /* updating */
          /* target can be found one single time in a list: stop */
          targetFound = true;
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
