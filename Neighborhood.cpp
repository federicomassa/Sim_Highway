
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
#include "ExtValue.h"
#include <iostream>

Neighborhood::Neighborhood(int t, const State& qT, const List<State>& qL,
                           const List<Hypothesis>& hL, const Vector<List<Hypothesis>, N_MANEUVER>& lastHL)
{
    targetID = t;
    qTarget = qT;

    Iterator<State> iq(qL);
    State q;
    
    while (iq(q))
      {
	qList.insHead(q);
      }
    
    
    hList = hL;
    lastHypLists = lastHL;
}

void Neighborhood::intersectionWith(const Neighborhood& n)
{
    /* error handling */
    if(targetID != n.targetID)
        error("Neighborhood", "cannot intersect with different targetID");
    
    if(this == &n)
        return;


    // ========== hypList merging: probably USELESS, replaced by possibleHypLists ============
    Hypothesis hyp;
    int nHyp = hList.count();
    for(int i = 0; i < nHyp; i++)
    {
        hList.extrHead(hyp);
        /* get corresponding hypothesis */
        Hypothesis tmpHyp;
        if(n.hList.find(hyp, tmpHyp))
        {
            bool valid = true;
            /* intersection between negative areas */
            hyp.negative *= tmpHyp.negative;
            /* intersection between sub-hypothesis */
            List<Hypothesis::SubHypothesis>& shList = hyp.subHypList;
            Hypothesis::SubHypothesis sHyp;
            int nsHyp = shList.count();
            for(int j = 0; j < nsHyp; j++)
            {
                shList.extrHead(sHyp);
                Hypothesis::SubHypothesis tmpSHyp;
                if (tmpHyp.subHypList.find(sHyp, tmpSHyp))
                {
                    sHyp.positive *= tmpSHyp.positive;
                    if(sHyp.positive.isEmpty())
                    {
                        valid = false;
                        break;
                    }
                    shList.insTail(sHyp);
                }
            }
            if(valid)
	      hList.insTail(hyp);
        }
    }
    // ============== END OF hypList MERGING ======================

    // ========= Merging of possibleHypLists =============
    for (int i = 0; i < N_MANEUVER; i++)
      {
	List<Hypothesis>& tmpHL = lastHypLists[i];
	List<Hypothesis> ntmpHL = n.lastHypLists[i];
	Hypothesis hyp;
	int numHyp = tmpHL.count();
	for(int n = 0; n < numHyp; n++)
	  {
	    tmpHL.extrHead(hyp);
	    /* get corresponding hypothesis, excluding same maneuver case with -1 */
	    Hypothesis tmpHyp;
	    if(ntmpHL.find(hyp, tmpHyp) && hyp.eventID != -1)
	      {
		bool valid = true;
		/* intersection between negative areas */
		hyp.negative *= tmpHyp.negative;
		/* intersection between sub-hypothesis */
		List<Hypothesis::SubHypothesis>& shList = hyp.subHypList;
		Hypothesis::SubHypothesis sHyp;
		int nsHyp = shList.count();
		for(int j = 0; j < nsHyp; j++)
		  {
		    shList.extrHead(sHyp);
		    Hypothesis::SubHypothesis tmpSHyp;
		    if (tmpHyp.subHypList.find(sHyp, tmpSHyp))
		      {
			sHyp.positive *= tmpSHyp.positive;
			if(sHyp.positive.isEmpty())
			  {
			    valid = false;
			    break;
			  }
			shList.insTail(sHyp);
		      }
		  }
		if(valid)
		  tmpHL.insTail(hyp);
	      }
	    else if(hyp.eventID == -1)
	      tmpHL.insTail(hyp);
	  }
	/* sort hypotesis list */
	tmpHL.sort();

      }
    
    // ========= End of lastHypLists merging =========
    
    /* merging qList with n.qList */
    qList.join(n.qList);
    
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

RepLevel Neighborhood::getTargetReputation() const
{

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
  ExtBool possibleManeuver[N_MANEUVER];

  for (int i = 0; i < N_MANEUVER; i++)
    {
      possibleManeuver[i] = F;
      List<Hypothesis> tmpHL = lastHypLists[i];


      // No hypotheses -> stay false, next maneuver
      if (tmpHL.isEmpty())
	continue;

      possibleManeuver[i] = T;

      Iterator<Hypothesis> iHyp(tmpHL);
      Hypothesis tmpHyp;
      
      while(iHyp(tmpHyp))
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

	  
	  if (possibleManeuver[i] == T)
	    break;
	}
    }

  // Now calculate reputation
  //     Case 1: same maneuver
  
  if (targetManeuver == targetLastManeuver)
    {
      if (possibleManeuver[targetManeuver] == F)
	return FAULTY;

      ExtBool isCorrect = T;
      for (int i = 0; i < N_MANEUVER; i++)
	{
	  if (i == (int)targetManeuver)
	    continue;

	  isCorrect = isCorrect && !possibleManeuver[i];
	  
	}

      if (isCorrect == T)
	return CORRECT;
      else
	return UNCERTAIN;

    }

  // Case 2: maneuver transition
  else
    {
      switch(possibleManeuver[targetManeuver])
	{
	case T:
	  return CORRECT;
	  break;
	case U:
	  return UNCERTAIN;
	  break;
	case F:
	  return FAULTY;
	  break;
	}
      
    }
  
  
  return UNSET;
  
}

double Neighborhood::measure() const
{
    double mu = 0;
    Iterator<Hypothesis> i(hList);
    Hypothesis h;
    while(i(h))
        mu += h.measure();
    
    return mu;
}

ostream& operator<<(ostream& os, const Neighborhood& n)
{
    os << "Neighborhood {" << EndLine(EndLine::INC);
    os << "Target ID: " << n.targetID << EndLine();
    os << "Target's state: " << n.qTarget << EndLine();
    os << "State list: " << n.qList << EndLine();
    os << "Hypothesis list: " << n.hList << EndLine();
    os << "Measure (Mu): " << n.measure() << EndLine(EndLine::DEC);
    os << '}';

    return os;
}
