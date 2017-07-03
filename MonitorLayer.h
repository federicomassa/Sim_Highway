/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        MonitorLayer.h
 * DESCRIPTION: Interface of the MonitorLayer class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef MONITORLAYER_H
#define MONITORLAYER_H

#include "Monitor.h"
#include "systemTypes.h"
#include <string>

extern Logger monitorLog;

class Knowledge;

/*!
 * This class represents the monitor layer of the agent.
 */
class MonitorLayer
{
    friend class Image;
    /*!
     * \brief True if the monitor layer is active.
     */
    bool active;
    /*!
     * \brief The ID of the agent containig this module.
     */
    int agentID;
    /*!
     * \brief Monitor list.
     */
    List<Monitor*> monitorList;
    /*!
     * \brief Add a monitor for an agent.
     */
    Monitor* addMonitor(int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<State>& qList);
    /*!
     * \brief Remove a monitor.
     */
    bool removeMonitor(int t);
    /*!
     * \brief Search for a Monitor.
     */
    Monitor* lookFor(int t);


public:
    /*!
     * \brief Default constructor.
     */
    MonitorLayer() { agentID = -1; active = false;}
    /*!
     * \brief Destructor.
     */
    ~MonitorLayer();
    /*!
     * \brief Set Agent ID.
     */
    void setID(int i) { agentID = i; }
    /*!
     * \brief Activate monitoring.
     */
    void activate() { active = true; }
    /*!
     * \brief Return active value.
     */
    bool isActive() const { return active; }
    /*!
     * \brief Run a single simulation step.
     */
    void run(const List<Sensing>& sList, const State& agentQ, const Maneuver& agentManeuver, const Area& obs);
    /*!
     * \brief Get all monitor's simpler hypothesis.
     */
    void getHypothesis(List<Hypothesis>& hList) const;
    /*!
     * \brief Build a list of neighborhood.
     */
    void buildNeighborhoodList(List<Neighborhood>& nList) const;
    void buildKnowledge(Knowledge& k) const;
    List<Monitor*>& getMonitorList() {return monitorList;}
    const List<Monitor*>& getMonitorList() const {return monitorList;}

};

#endif
