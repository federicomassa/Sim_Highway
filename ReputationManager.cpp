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

void ReputationManager::
    merge(const List<Message<List<Neighborhood> > >& msgList)
{
	Iterator<Message<List<Neighborhood> > > mi(msgList);
	Message<List<Neighborhood> > tmpMsg;
	while(mi(tmpMsg))
	{
		List<Neighborhood> tmpNL;
		tmpMsg.getBody(tmpNL); /* body extraction */
        singleMerge(tmpNL);
	}
}

void ReputationManager::init(Channel<List<Neighborhood> >* rC, int a)
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
                                         const List<Neighborhood>& nL)
{
    if(CONF.debug)
        LOG.s << "Setting current parameters" << EndLine(EndLine::INC);

    q = s;
    nList = nL;

    if(CONF.debug)
    {
        LOG.s << "Current state: " << q << EndLine();
        LOG.s << "Current knowledge: " << nList << EndLine(EndLine::DEC);
    }
}

void ReputationManager::singleMerge(List<Neighborhood>& nL)
{
    Iterator<Neighborhood> ni(nL);
    Neighborhood tmpNeigh;
    while(ni(tmpNeigh))
    {
        Iterator<Neighborhood> myni(nList);
        Neighborhood myTmpNeigh;
        bool targetFound = false;
        while(myni(myTmpNeigh))
            if(myTmpNeigh == tmpNeigh)
            {
                Neighborhood auxNeigh = myTmpNeigh; /* working on a copy */
                auxNeigh.intersectionWith(tmpNeigh); /* merging */
                nList.updateInfo(myTmpNeigh, auxNeigh); /* updating */
                /* target can be found one single time in a list: stop */
                targetFound = true;
                break;
            }
        if(!targetFound) //FIXME... solo nel merge: treat the case "agent too far"
            nList.insHead(tmpNeigh); /* insert a ``new'' target */
    }
}

void ReputationManager::getAgentsReputation(List<Reputation>& repList) const
{
	Iterator<Neighborhood> myni(nList);
	Neighborhood myTmpNeigh;

	while(myni(myTmpNeigh))
	{
		Reputation r;
		r.targetID = myTmpNeigh.getTargetID();
		r.qTarget = myTmpNeigh.getTargetState();
		r.level = myTmpNeigh.getTargetReputation();

		repList.insHead(r);
	}
}
