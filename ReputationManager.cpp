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

  
  Iterator<Neighborhood> itr(nList);
  Neighborhood n;

  while(itr(n))
    {
      Reputation* rep = findReputation(n.getTargetID());
      if (rep == 0)
	continue;

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

Reputation* ReputationManager::findReputation(const int& id)
{
  Reputation* r = 0;

  for (int i = 0; i < repList.count(); i++)
    {
      repList.getElem(r, i);

      if (r == 0)
	error("ReputationManager::findReputation", "Reputation list invalid");
      
      if (r->targetID != id)
	continue;

      /* if found matching ID */
      break;
    }

  return r;
  
}

const Reputation* ReputationManager::findReputation(const int& id) const
{
  const Reputation* r = 0;

  for (int i = 0; i < repList.count(); i++)
    {
      repList.getElem(r, i);

      if (r == 0)
	error("ReputationManager::findReputation", "Reputation list invalid");
      
      if (r->targetID != id)
	continue;

      /* if found matching ID */
      break;
    }

  return r;
  
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
      
      /* for each received sensor data entry */
      while (sItr(s))
	{
	  Vector<double, 2> position;
	  position[0] = s.x;
	  position[1] = s.y;

	  if (neg.contains(position))
	    {
	      record->result = F;
	      resultChanged = true;
	      levelChanged = true;
	      break;
	    }
		  
		
	}


      /* if it gets here a vehicle was not found in the negative area */
      neg = neg - recvObs;
      
      
      /* ============ POSITIVE AREAS HANDLING ================== */
      
      Area* posElement = 0;

      ExtBool isPositiveEventVerified = T;
      for (int i = 0; i < pos.count(); i++)
	{
	  pos.getElem(posElement, i);
	  
	  *posElement = *posElement - recvObs;

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

	  if (isVehiclePresentInPositiveArea == F)
	    {
	      if ((*posElement).isEmpty())
		{
		  record->result = F;
		  isPositiveEventVerified = F;
		  resultChanged = true;
		  levelChanged = true;
		  break;
		}
	      else
		isVehiclePresentInPositiveArea = U;
	    }
	  
	  
	  isPositiveEventVerified = isPositiveEventVerified && isVehiclePresentInPositiveArea;

	  
	  
	}
      
      /* if the sender was able to see everything in that area the negative condition holds true,
	 whereas if he does not see everything it is still uncertain */
      if (!neg.isEmpty())
	break;      

      if (!(isPositiveEventVerified == T))
	break;
      


      record->result = T;
      resultChanged = true;
      levelChanged = true;
    }

  if (levelChanged)
    rep->reEvaluateLevel();
  
}
