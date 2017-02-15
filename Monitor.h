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
#include "Tensor5.h"

extern Logger monitorLog;

/*!
 * This class represent a monitor module embedded on an agent and used for the
 * observation of the behavior of another agent.
 */
class Monitor
{
    friend class Image;
    friend class MonitorLayer; //FIXME only debug
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
    Maneuver targetLastManeuver;

    /* Store real maneuver at the beginning and at the end of the prediction time span */
    Maneuver realInitialManeuver;
    Maneuver realFinalManeuver;
    /*!
     * \brief Predicted states of the monitored agent.
     */
    Vector<State, N_MANEUVER> possibleStates;
    /*!
     * \brief Predicted maneuvers of the monitored agent.
     */
    Vector<bool, N_MANEUVER> possibleManeuvers;

    /* Time steps since beginning of prediction */
    int timeStepsCount;
    
    /* Predicted states with different maneuvers and hypotheses on hidden vehicle */
    Vector<List<Tensor5<Sensing> >, N_MANEUVER> monitorPrediction;

    /* Errors on prediction */
    Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER> errors;
    
    
    /*!
     * \brief Possible hypothesis associated to the predicted maneuvers and
     *        states.
     */
    List<Hypothesis> hypothesisList;
    /*!
     * \brief Hypothesis associated to the predicted maneuvers and states.
     */
    Vector<List<Hypothesis>, N_MANEUVER> possibleHypLists;
    Vector<List<Hypothesis>, N_MANEUVER> lastHypLists;
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
    Monitor(int a, int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<State>&);
    /*!
     * \brief Destructor.
     */
    ~Monitor() { }

    /* agent Q when this monitor started last prediction */
    State lastPredictionAgentQ;
    State predictionAgentQ;

    
    /*!
     * \brief Predict possible states for monitored agent.
     */
    void predictStates(const List<Sensing>&, const State&, const Maneuver&);
    /*!
     * \brief Detect events for target agent and predict possible maneuvers for
     *        monitored agent.
     */
    void preDetectManeuver(const State& q, const Maneuver& sigma);
    void predictManeuvers(const List<State>& qList, const Area& obs);
    
    void increaseCounter() {
      if (timeStepsCount < CONF.nTimeSteps)
	timeStepsCount++;
      else
	timeStepsCount = 0;
    }

    int getCountdown()
    {
      /* +1 because after detection it does not predict right away, but waits for 1 step */
      if (timeStepsCount >= 0)
	return (CONF.nTimeSteps - timeStepsCount + 1);
      else
	return (-timeStepsCount);
    }

    int getTimeCount() {return timeStepsCount;}

    void setTimeCount(const int& cnt)
    {
      timeStepsCount = cnt;
    }

    /*!
     * \brief Detect monitored agent current maneuver.
     */
    void detectManeuver(const State&, const State&);
    /*!
     * \brief Return targetLocked value.
     */
    bool isLocked() const { return targetLocked; }
    /*!
     * \brief Return targetID.
     */
    /* Return true if nTimeSteps have passed since last prediction */
    bool isReadyToPredict() {return (timeStepsCount == 0);}
    bool isReadyToDetect() {return (timeStepsCount == CONF.nTimeSteps);}
    bool isReadyForHypotheses() {return hypReady;}
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

    /* Accessors */
    void getMonitorPrediction(Vector<List<Tensor5<Sensing> >, N_MANEUVER>& v) {v = monitorPrediction;}
    void setMonitorPrediction(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& v) {monitorPrediction = v;}

    void getErrors(Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& err) {err = errors;}
    void setErrors(const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& err) {errors = err;}
    
    void getTargetQ(State& q) {q = targetQ;}

    /* set real maneuver, useful for comparisons */
    void setRealInitialManeuver(const Maneuver& sigma) {realInitialManeuver = sigma;}
    void setRealFinalManeuver(const Maneuver& sigma) {realFinalManeuver = sigma;}
};

#endif
