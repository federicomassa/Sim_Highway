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


extern Logger LOG;
extern const Configuration CONF;

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
    Channel<List<Neighborhood> >* repChan;
    /*!
     * \brief My agent identifier.
     */
    int agentID;
    /*!
     * \brief My current state.
     */
    State q;
    /*!
     * \brief My neighborhood list -- i.e. my knowledge or simply my
     *        ``neighborhood''.
     */
    List<Neighborhood> nList;
    /*!
     * \brief Merge my knowledge with information received from other agents.
     *
     * @param msgList list containing neighborhood lists of my neighbors.
     */
    void merge(const List<Message<List<Neighborhood> > >& msgList);
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
    void init(Channel<List<Neighborhood> >* rC, int a);
    /*!
     * \brief Set my state and my knowledge every simulation step.
     *
     * @param s my current state.
     * @param nL my current knowledge -- i.e. according to the vision of the
     *           monitor layer.
     */
    void setCurrentParams(const State& s, const List<Neighborhood>& nl);
    /*!
     * \brief Broadcast information every communication round.
     */
    void sendForConsensus()
    {
        repChan->sendBroadcast(agentID, q, nList); /* send my knowledge */
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
        List<Message<List<Neighborhood> > >msgList;
        repChan->recvBroadcast(agentID, q, msgList); /* receive information */
        merge(msgList); /* try to improve my knowledge */
    }
    /*!
     * \brief Merge my knowledge with the knowledge received from another agent.
     *
     * @param nL neighborhood list of my neighbor.
     */
    void singleMerge(List<Neighborhood>& nl);
    /*!
     * \brief Get neighborhood list reconstructed by the reputation manager.
     *
     * @param nL neighborhood list -- filled by this method.
     */
    void getNeighborhoodList(List<Neighborhood>& nl) const { nl = nList; };
    /*!
     * \brief Get reputation of all current visible agents.
     *
     * @param repList reputation list -- filled by this method.
     */
    void getAgentsReputation(List<Reputation>& repList) const;
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
