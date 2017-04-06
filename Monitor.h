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
#include "ActionManager.h"
#include "RuleMonitor.h"


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

    /* Memory of the target and neighbors trajetory */
    Vector<Sensing, 10> monitorStates;
    Vector<List<Sensing>, 10> neighStates;

    /* ActionManager and RuleMonitor */
    ActionManager aMan;
    RuleMonitor rMon;

    int lastRunTime;
public:
    /*!
     * \brief Constructor.
     */
    Monitor(int a, int t);
    /*!
     * \brief Destructor.
     */
    ~Monitor() { }

    /* Run monitor */
    void run(const Sensing&, const List<Sensing>&, const Area&);

    
    int getTargetID() const { return targetID; }
    /*!
     * \brief Return agentID.
     */
    int getAgentID() const { return agentID; }

    /* accessor to action manager */
    const ActionManager* getActionManager() const {return &aMan;}

    /* accessor to rule monitor */
    const RuleMonitor* getRuleMonitor() const {return &rMon;}

    const int& getLastRunTime() const {return lastRunTime;}
    
};

#endif
