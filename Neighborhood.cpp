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

Neighborhood::Neighborhood(int t, const State& qT, const List<State>& qL,
                           const List<Hypothesis>& hL)
{
    targetID = t;
    qTarget = qT;
    qList = qL;
    hList = hL;
}

void Neighborhood::intersectionWith(const Neighborhood& n)
{
    /* error handling */
    if(targetID != n.targetID)
        error("Neighborhood", "cannot intersect with different targetID");
    
    if(this == &n)
        return;
    
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
    
    /* merging qList with n.qList */
    qList.join(n.qList);
    
    /* sort hypotesis list */
    hList.sort();
}

RepLevel Neighborhood::getTargetReputation() const
{
    if(hList.isEmpty())
        return FAULTY;
    
    Iterator<Hypothesis> i(hList);
    Hypothesis hyp;
    while(i(hyp))
        if(!hyp.subHypList.isEmpty() || !hyp.negative.isEmpty())
            return UNCERTAIN;

    return CORRECT;
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
