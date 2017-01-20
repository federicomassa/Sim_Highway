/* 
   AUTHOR = FEDERICO MASSA, ADRIANO FAGIOLINI
   DATE = 11.01.2017
   DESCRIPTION = Class that predicts the state space of a vehicle after N simulation steps
   A vehicle is hypothesized in the hidden area with a uniform state distribution. The other vehicles are
   simulated as in a uniform motion.

*/

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include "Sensing.h"
#include "State.h"
#include "List.h"
#include "PhysicalLayer.h"
#include "DynVector.h"
#include "Tensor4.h"

class Predictor
{
  // ID of agent
  const int agentID;
  // state of observer at current time
  const State iAgentState;
  // sigma of observer at current time
  const Maneuver agentManeuver;
  // initial state of monitor
  //  const Sensing iMonitorState;
  // states as seen from the observer
  const List<Sensing> sList;
  // ID of monitor
  const int monitorID;
  // number of time steps to simulate. DUMMY for now
  const int nTimeSteps;
  
  // stores the current simulated time step. When currentStep == nTimeSteps, stop simulation
  int currentStep;

  /* vector containing the possible outcomes of the vehicles at current time, for the different monitor maneuvers
     External vector is for different monitor maneuvers, second vector is for vehicles, list
     is for the hidden area rectangles, internal tensor is for different point on a grid.   */
  //  Vector<DynVector<List<Tensor4<Sensing> >, N_MANEUVER> currentState;

  /* Vector containing the possible outcomes of the agent and monitor at current time, for the different monitor maneuvers */
  Vector<Tensor4<State>, N_MANEUVER> agentState;
  Vector<List<Tensor4<Sensing> >, N_MANEUVER> monitorState;

  /* List of rectangles containing a tensor that contains all the possible hidden vehicle states in that rect, in the order x, y, theta, v */
  /* Unlike the other vehicles' vectors, this does NOT contain a prediction, but only a hypothesis. */
  Vector<List<Tensor4<Sensing> >, N_MANEUVER> hiddenState;
  
  // Physical Layer, used to run simulation. Not used for vehicles other than agent and monitor
  // because we hypothesize that they follow a uniform speed motion
  PhysicalLayer agentPLayer;
  PhysicalLayer monitorPLayer;

  double deltaX, deltaY, deltaTheta, deltaV;
  
 public:
  // Arguments: agent index, agent state, agent maneuver, list of sensor measurements, monitor index, simulation steps (now dummy)
  Predictor(const int&, const State&, const Maneuver&, const List<Sensing>&, const int&, const int&);

  /*  // delta of bins and low/high range of x, y, theta, v 
  setGridRange(const double&, const double&, const double&,
	       const double&, const double&, const double&,
	       const double&, const double&, const double&,
	       const double&, const double&, const double&)*/
    
  ~Predictor();

  //step simulation
  void run();

  //Accessors
  void getHidden(Vector<List<Tensor4<Sensing> >, N_MANEUVER>& v) {v = hiddenState;}
  void getMonitor(Vector<List<Tensor4<Sensing> >, N_MANEUVER>& v) {v = monitorState;}
  
};

#endif
