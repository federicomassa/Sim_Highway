/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Neighborhood.h
 * DESCRIPTION: Interfare for the Neighborhood class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "List.h"
#include "Hypothesis.h"
#include "Reputation.h"

class Neighborhood
{
    friend class Image;
    friend class Monitor;
    friend ostream& operator<<(ostream& os, const Neighborhood& n);
    friend bool operator==(const Neighborhood& n1, const Neighborhood& n2)
    {
        return n1.targetID == n2.targetID && n1.qTarget == n2.qTarget;
    }
    friend bool operator!=(const Neighborhood& n1, const Neighborhood& n2)
    {
        return !(n1 == n2);
    }
    friend int size(Neighborhood n)
    {
        return size(n.targetID) + size(n.qTarget) + size(n.qList) + size(n.hList);
    }
    int targetID;
    State qTarget;
    
    //contains all information about the vehicles 
    List<State> qList;
    
    List<Hypothesis> hList;
public:
    Neighborhood() { targetID = -1; }
    Neighborhood(int t, const State& qT, const List<State>& qL,
                 const List<Hypothesis>& hL);
    ~Neighborhood() { }
    void intersectionWith(const Neighborhood& n);
    int getTargetID() const { return targetID; }
    State getTargetState() const { return qTarget; }
    RepLevel getTargetReputation() const;
    double measure() const;
};

#endif
