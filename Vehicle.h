#ifndef VEHICLE_H
#define VEHICLE_H

#include "ReputationManager.h"
#include "MonitorLayer.h"

class Knowledge;

class Vehicle
{
    friend class Image;
    Automaton automaton;
    PhysicalLayer pLayer;
    MonitorLayer mLayer;
    ReputationManager repMan;
    int idx;
    /* set reputation manager's current parameters */
    void setRM();
    /* List of sensor measurements */
    List<Sensing> sList;
public:
    /* constructor */
    Vehicle() { idx = -1; }
    /* destructor */
    ~Vehicle() { }
    /* initialization */
    void init(const State&, const Parms&);
    /* set vehicle identifier */
    void setID(int index)
    {
        idx = index;
        automaton.setID(index);
        mLayer.setID(index);
        pLayer.setID(index);
    }
    /* initialize the reputation manager */
    void initRM(Channel<Knowledge>* rC) { repMan.init(rC, idx, this); }
    /* execute the first step of the reputation manager's task */
    void sendRM() { if (repMan.isActive()) repMan.sendForConsensus(); }
    /* execute the reputation manager's core-task */
    void recvRM() { if (repMan.isActive()) repMan.recvForConsensus(); }
    /* get pointer to reputation manager */
    const ReputationManager& getRM() const {return repMan;}
    /* get continuous state q */
    State getQ() const { return pLayer.getQ(); }

    /* set continuous state q - for predictor */
    void setQ(const State& q) {pLayer.init(q, q.v);}

    /* get vehicle identifier */
    int getID() const { return idx; }
    Parms getParms() const { return pLayer.getParms(); }
    Maneuver getManeuver() const { return automaton.getManeuver(); }
    List<Sensing>& getSList() {return sList;}
    const List<Sensing>& getSList() const {return sList;}
    /* check if (part of) the vehicle state q belongs to the Area */
    bool inArea(const Area& a) const
    {
        return a.contains(pLayer.getQ().toPoint());
    }
    void preRun(const List<Sensing>& sList);
    void evolveMonitor();
    void run() { pLayer.updateQ(); }
    void activateMonitorLayer() { mLayer.activate(); }
    void activateReputationManager() { repMan.activate(); }
    void getHypothesis(List<Hypothesis>& hList);
    void getNeighborhoodList(List<Neighborhood>& nList) const
    {
        repMan.getNeighborhoodList(nList);
    }
    void getAgentReputationList(List<Reputation>& repList) const
    {
        repMan.getAgentsReputation(repList);
    }
    MonitorLayer* getMonitorLayer() {return &mLayer;}
    const MonitorLayer* getMonitorLayer() const {return &mLayer;}

    /* Send wait request from reputation manager to monitor layer to
     sync prediction */
    void shareWaitingList();
    const Knowledge& getKnowledge()
    {
        return repMan.getKnowledge();
    }
    const Vehicle& operator=(const Vehicle&);
};

#endif

