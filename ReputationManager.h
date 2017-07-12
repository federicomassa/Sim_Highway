/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        ReputationManager.h
 * DESCRIPTION: Interface for the ReputationManager class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef REPUTATIONMANAGER_H
#define REPUTATIONMANAGER_H

#include "Channel.h"
#include "Configuration.h"
#include "Logger.h"
#include "Neighborhood.h"
#include "Knowledge.h"

extern Logger LOG;
extern Logger monitorLog;
extern const Configuration CONF;

class Vehicle;
template<typename T> class Tensor5;

/*!
 * This class represents the reputation manager; such module is a component
 * of the local IDS embedded on the agent architecture.
 * The reputation manager merges information received from different agents
 * and tries to refine the knowledge built by its local monitor.
 * In reality, a task of this object is also to transmit its ``neighborhood''
 * and then to receive ``neighborhood'' messages.
 */
class ReputationManager
{
  /*!
   * \brief True if the monitor layer is active.
   */
  bool active;
  /*!
   * \brief Pointer to the channel used to exchange ``neighborhood''.
   */
  Vehicle* vehicle;

  Channel<Knowledge>* repChan;
  /*!
   * \brief My agent identifier.
   */
  int agentID;
  /*!
   * \brief My current state.
   */
  State q;

  /* Knowledge available to the vehicle */
  Knowledge knowledge;

  /*!
   * \brief Merge my knowledge with information received from other agents.
   *
   * @param msgList list containing neighborhood lists of my neighbors.
   */
  void merge(const List<Message<Knowledge> >& msgList);

  void checkNewVehicle(const Sensing& newVehicle, const int& targetID);
  void compareVehicleWithHidden(const Sensing& newVehicle, const int& targetID, Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses);
  void removeIncompatibleHypotheses(Vector<List<Tensor5<bool> >, N_MANEUVER>& hypothesesLeft, const Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibleHypotheses);
  Vector<List<Tensor5<bool> >, N_MANEUVER> findCompatibleHypotheses(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& hidden, const Sensing& newVehicle);

  Vehicle* getVehicle() {return vehicle;}

public:
  /*!
   * \brief Default constructor.
   */
  ReputationManager() { repChan = NULL; agentID = -1; }
  /*!
   * \brief Destructor.
   */
  ~ReputationManager() { }
  /*!
   * \brief Initialize fundamental members.
   *
   * This method initializes the ``fixed'' members, i.e. those members
   * that don't change every step, like the channel and the agent identifier.
   * @param rC pointer to the channel.
   * @param a agent identifier.
   */
  void init(Channel<Knowledge>* rC, int a, Vehicle* v = NULL);
  /*!
   * \brief Set my state and my knowledge every simulation step.
   *
   * @param s my current state.
   * @param nL my current knowledge -- i.e. according to the vision of the
   *           monitor layer.
   */
  Channel<Knowledge>* const getChannel() const {return repChan;}

  void setCurrentParams(const State& s, const Knowledge& k);
  /*!
   * \brief Broadcast information every communication round.
   */
  void sendForConsensus()
  {
    repChan->sendBroadcast(agentID, q, knowledge); /* send my ID and knowledge */
  }
  /*!
   * \brief Execute the reputation manager's core-task.
   *
   * This method receives information from other agents and then tries to
   * merge such information in order to refine my knowledge.
   */
  void recvForConsensus()
  {
    /* message list that will contain neighborhood lists of my neighbors */
    List<Message<Knowledge> >msgList;
    repChan->recvBroadcast(agentID, q, msgList); /* receive information */
    merge(msgList); /* try to improve my knowledge */
  }
  /*!
   * \brief Merge my knowledge with the knowledge received from another agent.
   *
   * @param nL neighborhood list of my neighbor.
   */
  void singleMerge(const Knowledge& k);
  /*!
   * \brief Get neighborhood list reconstructed by the reputation manager.
   *
   * @param nL neighborhood list -- filled by this method.
   */
  void getNeighborhoodList(List<Neighborhood>& nl) const { nl = knowledge.nList; };
  /*!
   * \brief Get reputation of all current visible agents.
   *
   * @param repList reputation list -- filled by this method.
   */
  void getAgentsReputation(List<Reputation>& repList) const;

  /* Get knowledge */
  const Knowledge& getKnowledge() const {return knowledge;}

  /*!
   * \brief Activate behavior classification.
   */
  void activate() { active = true; }
  /*!
   * \brief Return active value.
   */
  bool isActive() const { return active; }
};

#endif
