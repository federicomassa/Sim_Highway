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

void ReputationManager::merge(const List<Message<Knowledge> >& msgList)
{
  Iterator<Message<Knowledge> > mi(msgList);
  Message<Knowledge> tmpMsg;
  while(mi(tmpMsg))
    {
      Knowledge tmpK;
      
      /* ignore your own message */
      if (tmpK.agentID == agentID)
	continue;
      
      tmpMsg.getBody(tmpK); /* body extraction */
      singleMerge(tmpK);
    }
  
}

void ReputationManager::init(Channel<Knowledge>* rC, int a)
{
    /* error handling */
    if(rC == NULL)
        error("ReputationManager", "pointer to the channel is NULL");
    if(a < 0)
        error("ReputationManager", "NEGATIVE agent identifier");

    /* initialization */
    repChan = rC;
    agentID = a;
}

void ReputationManager::setCurrentParams(const State& s,
                                         const Knowledge& k)
{
    if(CONF.debug)
        LOG.s << "Setting current parameters" << EndLine(EndLine::INC);

    q = s;
    knowledge = k;

    if(CONF.debug)
    {
        LOG.s << "Current state: " << q << EndLine();
        LOG.s << "Current knowledge: " << knowledge.nList << EndLine(EndLine::DEC);
    }
}

void ReputationManager::singleMerge(const Knowledge& k)
{
  
  const List<Neighborhood>& nList = k.nList;
  const Area& recvObs = k.visibleArea;

  
  Iterator<Neighborhood> nItr(nList);
  Neighborhood n;
  
  while (nItr(n))
    {
      
      Reputation* rep = findReputation(n.getTargetID());
      if (rep == 0)
	continue;

      mergeNeighborhood(n); 
      
      /* Now I need to see if in the received knowledge there is 
	 something about my uncertainty areas */
      mergeReputation(rep, n, recvObs);
      
    }

}

void ReputationManager::getAgentsReputation(List<Reputation>& repL) const
{
  repL.purge();
  
  Iterator<Neighborhood> myni(knowledge.nList);
  Neighborhood myTmpNeigh;
  
  while(myni(myTmpNeigh))
    {
      Reputation r;
      r.targetID = myTmpNeigh.getTargetID();
      r.sTarget = myTmpNeigh.getTargetSensing();
      r.level = myTmpNeigh.getTargetReputation();
      
      repL.insHead(r);
    }
}

/* Add received sensor data to your set, so that you can also share it to others */
void ReputationManager::mergeNeighborhood(const Neighborhood& n)
{
  
  Neighborhood* myN = findNeighborhood(n.getTargetID());

  /* if you did not know about a vehicle, do not add, you do not see it, you don't care. */
  if (myN == 0)
    return;

  const List<Sensing>& sList = n.getSList();
  List<Sensing>& mySList = myN->getSList();
  
  /* for each received vehicle data */
  Iterator<Sensing> sItr(sList);
  Sensing s;

  while(sItr(s))
    {
      /* for each of my vehicle data, if you already have it nothing (TODO or improve measurements??) 
       if you don't, add it to the list */
      if (!foundAgent(s, mySList))
	mySList.insTail(s);
      
    }
 
}

bool ReputationManager::foundAgent(const Sensing& s, const List<Sensing>& mySList)
{
  Iterator<Sensing> mySItr(mySList);
  Sensing myS;

  bool found = false;
  
  while (mySItr(myS))
    {
      if (s.agentID == myS.agentID)
	{
	  found = true;
	  break;
	}
    }

  return found;
  
}

Reputation* ReputationManager::findReputation(const int& id)
{
  Reputation* r = 0;

  bool found = false;
  
  for (int i = 0; i < repList.count(); i++)
    {
      repList.getElem(r, i);

      if (r == 0)
	error("ReputationManager::findReputation", "Reputation list invalid");
      
      if (r->targetID != id)
	continue;

      /* if found matching ID */
      found = true;
      break;
    }

  if (found)
    return r;
  else
    return 0;

  return 0;
  
}

const Reputation* ReputationManager::findReputation(const int& id) const
{
  const Reputation* r = 0;

  bool found = false;
  
  for (int i = 0; i < repList.count(); i++)
    {
      repList.getElem(r, i);

      if (r == 0)
	error("ReputationManager::findReputation", "Reputation list invalid");
      
      if (r->targetID != id)
	continue;

      /* if found matching ID */
      found = true;
      break;
    }

  if (found)
    return r;
  else
    return 0;

  return 0;
  
}

Neighborhood* ReputationManager::findNeighborhood(const int& id)
{
  Neighborhood* n = 0;

  bool found = false;
  
  for (int i = 0; i < knowledge.nList.count(); i++)
    {
      knowledge.nList.getElem(n, i);

      if (n == 0)
	error("ReputationManager::findNeighborhood", "Neighborhood list invalid");
      
      if (n->getTargetID() != id)
	continue;

      /* if found matching ID */
      found = true;
      break;
    }

  if (found)
    return n;
  else
    return 0;

  return 0;
  
}

const Neighborhood* ReputationManager::findNeighborhood(const int& id) const
{
  const Neighborhood* n = 0;

  bool found = false;
  
  for (int i = 0; i < knowledge.nList.count(); i++)
    {
      knowledge.nList.getElem(n, i);

      if (n == 0)
	error("ReputationManager::findNeighborhood", "Neighborhood list invalid");
      
      if (n->getTargetID() != id)
	continue;

      /* if found matching ID */
      found = true;
      break;
    }

  if (found)
    return n;
  else
    return 0;

  return 0;
  
}


/* Helper function to modify our rep records with received knowledge */
void ReputationManager::mergeReputation(Reputation* rep, const Neighborhood& n, const Area& recvObs)
{
  if (rep == 0)
    error("ReputationManager::mergeReputation", "Reputation pointer argument cannot be null");

  
  bool levelChanged = false;
  
  /* My history of reputation records */
  List<RepRecord>& history = rep->getHistory();
  RepRecord* record = 0;

  /* received sList */
  const List<Sensing>& sList = n.getSList(); 

  /* for each rule processed */
  for (int i = 0; i < history.count(); i++)
    {
      history.getElem(record, i);
      
      /* for now we only consider rules evaluated at this step */
      if (record->evalTime < now)
	break;

      /* merging is useful only if we had an uncertainty */
      if (record->result != U)
	continue;
      
      /* these are the areas of the present rule */
      Area& neg = record->negativeArea;
      List<Area>& pos = record->positiveArea;

      /* First check negative areas */
      Iterator<Sensing> sItr(sList);
      Sensing s;

      bool resultChanged = false;
      
      /* ============= NEGATIVE AREAS HANDLING ================ */
      ExtBool isNegativeEventVerified = U;

      /* no negative subevents case */
      if (neg.isEmpty())
	isNegativeEventVerified = T;
      else
	{
	  /* for each received sensor data entry */
	  while (sItr(s))
	    {
	      Vector<double, 2> position;
	      position[0] = s.x;
	      position[1] = s.y;
	      
	      if (neg.contains(position))
		{
		  if (agentID == 0 && n.getTargetID() == 1)
		    ResultLog.s << "!!! " << record->rule << EndLine(ResultLog.getIndentation());
		    
		  Area emptyArea;
		  neg = emptyArea;
		  record->result = F;
		  isNegativeEventVerified = F;
		  resultChanged = true;
		  break;
		}
	      
	      
	    }
	  
	  if (isNegativeEventVerified == U)
	    {
	      neg = neg - recvObs;
	      
	      /* received data does not contain vehicle in negative area and it sees it all */
	      if (neg.isEmpty())
		{
		  isNegativeEventVerified = T;
		}
	    }

	}
      /* ============ POSITIVE AREAS HANDLING ================== */

      ExtBool isPositiveEventVerified = T;
      
      if (pos.count() == 0)
	isPositiveEventVerified = T;
      else
	{
	  Area* posElement = 0;
	  
	  
	  for (int i = 0; i < pos.count(); i++)
	    {
	      pos.getElem(posElement, i);
	      
	      
	      
	      /* reset iterator */
	      sItr.initialize(sList);
	      
	      ExtBool isVehiclePresentInPositiveArea = F;
	      /* for each received sensor data entry */
	      while (sItr(s))
		{
		  Vector<double, 2> position;
		  position[0] = s.x;
		  position[1] = s.y;
		  
		  if (posElement->contains(position))
		    {
		      isVehiclePresentInPositiveArea = T;
		      break;
		    }
		  
		  
		  
		  
		}
	      
	      (*posElement) = (*posElement) - recvObs;
	      
	      if (isVehiclePresentInPositiveArea == F)
		{
		  if (posElement->isEmpty())
		    {
		      Area emptyArea;
		      (*posElement) = emptyArea; 
		      record->result = F;
		      isPositiveEventVerified = F;
		      resultChanged = true;
		    }
		  else
		    isVehiclePresentInPositiveArea = U;
		}
	      
	      
	      isPositiveEventVerified = isPositiveEventVerified && isVehiclePresentInPositiveArea;
	      
	  
	  
	    }
	}
      
      if (isPositiveEventVerified == T && isNegativeEventVerified == T)
	{
	  record->result = T;
	  resultChanged = true;
	}

      if(resultChanged)
	{
	  levelChanged = true;
	  
	  Area emptyArea;
	  neg = emptyArea;
	  
	  Area* posElement = 0;
	  for (int i = 0; i < pos.count(); i++)
	    {
	      pos.getElem(posElement, i);
	      (*posElement) = emptyArea;
	    }
		 
	}

      
    }

  
  if (levelChanged)
    rep->reEvaluateLevel();
  
}
