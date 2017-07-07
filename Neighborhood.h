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
#include "Sensing.h"

#include <map>

class Neighborhood
{
    friend class Image;
    friend class Monitor;
    friend class MonitorLayer; //FIXME only for debug
    
    friend std::ostream& operator<<(std::ostream& os, const Neighborhood& n);
    friend bool operator==(const Neighborhood& n1, const Neighborhood& n2)
    {
        return n1.targetID == n2.targetID;
    }
    friend bool operator!=(const Neighborhood& n1, const Neighborhood& n2)
    {
        return !(n1 == n2);
    }
    friend int size(Neighborhood n)
    {
        return size(n.targetID) + size(n.qTarget) + size(n.sList) + size(n.hList);
    }
    int targetID, agentID;
    State qTarget;
    
    //contains all information about the vehicles 
    List<Sensing> sList;
    
    List<Hypothesis> hList;
    std::map<Maneuver, Vector<List<Hypothesis>, N_MANEUVER> > lastHypLists;

    // Needed to merge hypotheses --- now dummy?
    Maneuver targetManeuver;
    Maneuver targetLastManeuver;

    List<Maneuver> maneuversLeft;
    List<Maneuver> lastManeuversLeft;


    
public:
    Neighborhood() { targetID = -1; }
    Neighborhood(int t, const State& qT, const List<Sensing>& sL,
                 const List<Hypothesis>& hL, const std::map<Maneuver, Vector<List<Hypothesis>, N_MANEUVER> >& possibleHL);
    ~Neighborhood() { }
    void intersectionWith(const Neighborhood& n);
    int getTargetID() const { return targetID; }
    int getAgentID() const { return agentID; }
    State getTargetState() const { return qTarget; }
    List<Sensing> getSList() const { return sList; }
    List<Hypothesis> getHList() const { return hList; }

    List<Maneuver>& getLastManeuversLeft() {return lastManeuversLeft;}
    List<Maneuver>& getManeuversLeft() {return maneuversLeft;}
    std::map<Maneuver, Vector<List<Hypothesis>, N_MANEUVER> >& getLastHypotheses() {return lastHypLists;}
    // One target reputation for each targetManeuver detected at the previous instant
    std::map<std::pair<Maneuver, Maneuver>, RepLevel> getTargetReputation() const;
    double measure() const;
};

#endif
