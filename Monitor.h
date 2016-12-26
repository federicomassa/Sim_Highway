/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Monitor.h
 * DESCRIPTION: Interface of the Monitor class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef MONITOR_H
#define MONITOR_H

#include "Automaton.h"
#include "Neighborhood.h"
#include "PhysicalLayer.h"
#include "Sensing.h"
#include "systemTypes.h"

/*!
 * This class represent a monitor module embedded on an agent and used for the
 * observation of the behavior of another agent.
 */
class Monitor
{
    friend class Image;
    /*!
     * \brief Redefinition of operator ==.
     */
    friend bool operator==(const Monitor& m1, const Monitor& m2)
    {
        return m1.targetID == m2.targetID;
    }
    /*!
     * \brief Redefinition of operator !=.
     */
    friend bool operator!=(const Monitor& m1, const Monitor& m2)
    {
        return !(m1 == m2);
    }
    /*!
     * \brief The ID of the agent containing this module.
     */
    int agentID;
    /*!
     * \brief The ID of the monitored agent.
     */
    int targetID;
    /*!
     * \brief The continuous state of the monitored agent.
     */
    State targetQ;
    /*!
     * \brief Last observed state of the monitored agent.
     */
    State targetLastQ;
    /*!
     * \brief Measured neighbors of target agent.
     */
    List<State> neighbors;
    /*!
     * \brief Last measured neighbors of target agent.
     */
    List<State> lastNeighbors;
    /*!
     * \brief Monitored agent parameters -- we suppose to be able to measure
     *        them.
     */
    Parms targetParms;
    /*!
     * \brief Estimated maneuver of the monitored agent.
     */
    Maneuver targetManeuver;
    /*!
     * \brief Predicted states of the monitored agent.
     */
    Vector<State, N_MANEUVER> possibleStates;
    /*!
     * \brief Predicted maneuvers of the monitored agent.
     */
    Vector<bool, N_MANEUVER> possibleManeuvers;
    /*!
     * \brief Possible hypothesis associated to the predicted maneuvers and
     *        states.
     */
    List<Hypothesis> hypothesisList;
    /*!
     * \brief Hypothesis associated to the predicted maneuvers and states.
     */
    Vector<List<Hypothesis>, N_MANEUVER> possibleHypLists;
    /*!
     * \brief A prototype of vehicle's automaton.
     */
    Automaton automaton;
    /*!
     * \brief A prototype of vehicle's physical layer.
     */
    PhysicalLayer pLayer;
    /*!
     * \brief Indicate if it's possible to build some knowledge about target
     *        agent.
     */
    bool targetLocked;
    bool hypReady;
public:
    /*!
     * \brief Constructor.
     */
    Monitor(int a, int t, const State& tQ, const Parms& tP, const Maneuver& tSigma);
    /*!
     * \brief Destructor.
     */
    ~Monitor() { }
    /*!
     * \brief Predict possible states for monitored agent.
     */
    void predictStates(/* For platoon */ List<State>);
    /*!
     * \brief Detect events for target agent and predict possible maneuvers for
     *        monitored agent.
     */
    void predictManeuvers(const List<State>& qList, const Area& obs);
    /*!
     * \brief Detect monitored agent current maneuver.
     */
    void detectManeuver(const State& q, const Maneuver& sigma);
    /*!
     * \brief Return targetLocked value.
     */
    bool isLocked() const { return targetLocked; }
    /*!
     * \brief Return targetID.
     */
    int getTargetID() const { return targetID; }
    /*!
     * \brief Return agentID.
     */
    int getAgentID() const { return agentID; }
    /*!
     * \brief Get first monitor hypothesis.
     */
    bool getFirstHypothesis(Hypothesis& hyp) const;
    /*!
     * \brief Build target estimated neighborhood.
     */
    bool buildNeighborhood(Neighborhood& n) const;
};

#endif
