
#ifndef AUTOMATON_H
#define AUTOMATON_H

#include "rules.h"

/*!
 * This class represents an automaton with discrete state sigma -- of type
 * Maneuver -- and a set of transitions, events and sub-events.
 * @see Transition
 * @see Event
 * @see SubEvent
 */
class Automaton
{
    /*!
     * \brief Class Monitor must be friend of this.
     */
    friend class Monitor;
    /*!
     * \brief A vector of sub-events.
     */
    Vector<SubEvent, N_SUB_EVENT> subEvents;
    /*!
     * \brief A vector of events.
     */
    Vector<Event, N_EVENT> events;
    /*!
     * \brief A vector of transitions.
     */
    Vector<Vector<Transition, N_MANEUVER>, N_MANEUVER> transitions;
    /*!
     * \brief A vector of reset function -- one for each maneuver.
     *
     * This is a vector of reset function. Each function updates automaton
     * internal variables based on the current discrete state and a continuous
     * state q.
     */
    Vector<ResetFunction, N_MANEUVER> resetFunctions;
    /*!
     * \brief The ID of the vehicle this automaton belongs to.
     */
    int agentID;
    /*!
     * \brief Indicates if this automaton has been initialized or not.
     */
    bool initialized;
    /*!
     * \brief Internal variables.
     */
    IntVars xi;
    /*!
     * \brief The discrete state of the automaton represents vehicle maneuver.
     */
    Maneuver sigma;
public:
    /*!
     * \brief Default constructor.
     */
    Automaton();
    /*!
     * \brief Destructor.
     */
    ~Automaton() { }
    /*!
     * \brief Initialize internal vars based on continuous state q.
     *
     * @param q continuous state.
     */
    void init(const State& q);
    /*!
     * \brief Set current maneuver to m.
     *
     * This method sets current automaton state to m and call the correct
     * reset function to update internal variables based on current discrete
     * state and vehicle continuous state q.
     * @param m new automaton discrete state.
     * @param q vehicle continuous state.
     */
    void setManeuver(Maneuver m, const State& q);
    /*!
     * \brief Detect event based on a list of neighbors states.
     *
     * This methods try to detects an event or a set of possible events based on
     * the continuous state of the vehicle this automaton belongs to and a list
     * of states representing the states of vehicle neighbors.
     * @param qSubj vehicle continuous state.
     * @param qList neighbors states list.
     * @param omniscent indicates if the event evaluation must be omniscent or
     *        not.
     * @return True if an event was detected, false otherwise.
     */
    bool detectEvents(const State& qSubj, const List<State>& qList,
                      bool omniscient);
    /*!
     * \brief Run an evolution step of the automaton.
     *
     * This method calls the method detectEvents() and, if an event is detected,
     * updates current automaton discrete state.
     * @param qSubj vehicle continuous state.
     * @param qList neighbors states list.
     */
    void run(const State& qSubj, const List<State>& qList);
    /*!
     * \brief Return current automaton discrete state.
     *
     * @return automaton discrete state.
     */
    Maneuver getManeuver() const { return sigma; }
    /*!
     * \brief Return current automaton internal variables.
     *
     * @return automaton internal variables.
     */
    IntVars getIntVars() const { return xi; }
    /*!
     * \brief Set automaton agentID.
     *
     * @param index automaton agentID.
     */
    void setID(int index) { agentID = index; }
};

#endif
