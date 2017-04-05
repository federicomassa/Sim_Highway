#ifndef VEHICLE_H
#define VEHICLE_H

#include "ReputationManager.h"
#include "MonitorLayer.h"
#include <fstream>

class Knowledge;

class Vehicle
{
  friend class Image;
  Automaton automaton;
  PhysicalLayer pLayer;
  MonitorLayer mLayer;
  ReputationManager repMan;
  int idx;
  Parms p;
  /* set reputation manager's current parameters */
  void setRM();
  /* List of sensor measurements */
  List<Sensing> sList;
 public:
  /* constructor */
  Vehicle();
  /* destructor */
  ~Vehicle();
  /* initialization */
  void init(const std::pair<State, Parms>&);
  /* set vehicle identifier */
  void setID(int index);
  /* initialize the reputation manager */
  void initRM(Channel<Knowledge>* rC) { repMan.init(rC, idx); }
  /* execute the first step of the reputation manager's task */
  void sendRM() { if(repMan.isActive()) repMan.sendForConsensus(); }
  /* execute the reputation manager's core-task */
  void recvRM() { if(repMan.isActive()) repMan.recvForConsensus(); }
  /* get pointer to reputation manager */
  const ReputationManager& getRM() const {return repMan;}
  /* get continuous state q */    
  State getQ() const;
  /* set continuous state q - for predictor */
  void setQ(const State& q);
  /* get vehicle identifier */
  int getID() const { return idx; }
  Parms getParms() const { return p; }
  Maneuver getManeuver() const { return automaton.getManeuver(); }
  List<Sensing>& getSList() {return sList;}
  /* check if (part of) the vehicle state q belongs to the Area */
  bool inArea(const Area& a) const;
  void preRun(const List<Sensing>& sList, const Area& obs);
  void evolveMonitor(const Area& obs);
  void run();
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
  const MonitorLayer* getMonitorLayer() const {return &mLayer;}
  /* Send wait request from reputation manager to monitor layer to
     sync prediction */
  void shareWaitingList();
  const Knowledge& getKnowledge()
  {
    return repMan.getKnowledge();
  }

  int getPixelWidth() const {return p.vehicleType.getWidth()*SCALE;}
  int getPixelHeight() const {return p.vehicleType.getHeight()*SCALE;}
};

#endif

