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
        return size(n.targetID) + size(n.sList);
    }
    int targetID, agentID;
    Sensing sTarget;
    
    //contains all information about the vehicles seen by the target (as visible from the monitor)
    List<Sensing> sList;
    
    
public:
    Neighborhood() { targetID = -1; }
    Neighborhood(int t, const Sensing&, const List<Sensing>&);
    void init(int t, const Sensing&, const List<Sensing>&);
    ~Neighborhood() { }
    void intersectionWith(const Neighborhood& n);
    int getTargetID() const { return targetID; }
    int getAgentID() const { return agentID; }
    const Sensing& getTargetSensing() const {return sTarget;}
    const List<Sensing>& getSList() const { return sList; }
    List<Sensing>& getSList() { return sList; }
    
    RepLevel getTargetReputation() const;

    const Neighborhood& operator=(const Neighborhood& n)
      {
	targetID = n.targetID;
	agentID = n.agentID;
	sTarget = n.sTarget;
	sList = n.sList;

	return *this;
      }
};

#endif
