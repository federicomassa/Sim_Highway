
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "Vehicle.h"
#include "Knowledge.h"
#include "Output.h"

extern const Configuration CONF;

class Environment
{
    friend class Image;
    /*!
     * \brief Channel used by the Reputation Manager to exchange ``neighborhood''.
     */
    Channel<Knowledge> repChannel;
    /* array of vehicles */
    Vehicle* v;
    /* number of vehicles */
    const int nV;
public:
    /* constructor */
    Environment(int n, double r, double p);
    Environment(const Environment&);
    /* destructor */
    ~Environment() { delete[] v; }
    /* vehicles initialization */
    void initVehicles(const List<State>&, const List<Parms>&);
    /* get vehicle state q */
    State getQ(int index) const;
    /* get number of vehicles */
    int getNVehicles() const {return nV;}
    /* get vehicle vector v */
    Vehicle* getVehicles() {return v;}
    const Vehicle* getVehicles() const {return v;}

    /* get vehicle maneuver m */
    Maneuver getManeuver(int index) const;
    /* returns i-th agent's observable area */
    void observableArea(int index, Area& obs, Area* hidden = NULL, const double& visible_distance = VISIBLE_DISTANCE) const;
    /* call a simulation step */
    void run();
    /* get a neighborhood list reconstructed by an omniscient observer */
    void omniscientNeighborhoodList(List<Neighborhood>& oNL);
    /* a consensus step */
    void consensusStep();
    /* activate a apecific monitor layer */
    void activateMonitorLayer(int index);
    /* activate a specific reputation manager */
    void activateReputationManager(int index);
    /* get vehicle's hypothesis */
    void getHypothesis(int index, List<Hypothesis>& hypList) const;
    /* output Neighborhoods measure */
    void outputNeighborhoodsMu(int cStep) const;
    /* output targets reputation level */
    void outputTargetsReputation(int cStep) const;
    /* output Neighborhoods communication overhead */
    void outputNeighborhoodsOverhead(int cStep) const;
    /* output vehicles states and maneuvers */
    void outputVehiclesStates() const;
    // accessor
    const Channel<Knowledge>& getChannel() const {return repChannel;}
};

#endif
