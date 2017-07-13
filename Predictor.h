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
#include "Area.h"
#include "PhysicalLayer.h"
#include "DynVector.h"
#include "Tensor5.h"
#include "SensorErrorSimulator.h"

extern Logger monitorLog;

class Predictor
{
  // ID of agent
  const int agentID;
  // state of observer at current time
  State iAgentState;
  // sigma of observer at current time
  Maneuver agentManeuver;
  // initial state of monitor
  //  const Sensing iMonitorState;
  // states as seen from the observer
  List<Sensing> sList;
  // ID of monitor
  const int monitorID;
  // number of time steps to simulate. DUMMY for now
  const int nTimeSteps;

  static const double compatibilityCut;
 
  /* vector containing the possible outcomes of the vehicles at current time, for the different monitor maneuvers
     External vector is for different monitor maneuvers, second vector is for vehicles, list
     is for the hidden area rectangles, internal tensor is for different point on a grid.   */
  //  Vector<DynVector<List<Tensor4<Sensing> >, N_MANEUVER> currentState;

  /* Vector containing the possible outcomes of the agent and monitor at current time, for the different monitor maneuvers */
  Vector<Tensor5<State>, N_MANEUVER> agentState;
  Vector<List<Tensor5<Sensing> >, N_MANEUVER> monitorState;
  Sensing initialMonitorState;
  /* Error due to the discretization of the hidden vehicle state space */
  /* External vector = monitor maneuvers 
     List: rectangles of hidden area 
  Tensor5: discretization of state space
  Vector<double, 4> error as produced by getError method */
  Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER> discrErrors;

  /* Errors due to the uniform speed motion hypothesis of the vehicles (except 
   monitored and observer) */
  Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER> uniformErrors;
  
  // Errors due to forward propagation of initial sensor errors
  Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER> forwardSensorErrors;
  
  /* List of rectangles containing a tensor that contains all the possible hidden vehicle states in that rect, in the order x, y, theta, v */
  /* Unlike the other vehicles' vectors, this does NOT contain a prediction, but only a hypothesis. */
  Vector<List<Tensor5<Sensing> >, N_MANEUVER> hiddenState;
  
  // Physical Layer, used to run simulation. Not used for vehicles other than agent and monitor
  // because we hypothesize that they follow a uniform speed motion
  PhysicalLayer agentPLayer;
  PhysicalLayer monitorPLayer;

  //Area in which hidden vehicle is mapped
  Area mappingArea;
  // result of the compatibilities of the predictions
  Vector<List<Tensor5<double> >, N_MANEUVER> compatibility;


  double deltaX, deltaY, deltaTheta, deltaV, deltaDesiredV;
  
  SensorErrorSimulator sensorSimul;

 public:
  // Arguments: agent index, agent state, agent maneuver, list of sensor measurements, monitor index, simulation steps
  Predictor(const int&, const int&, const int&);
  void init(const State&, const Maneuver&, const List<Sensing>&);
  
  /*  // delta of bins and low/high range of x, y, theta, v 
  setGridRange(const double&, const double&, const double&,
	       const double&, const double&, const double&,
	       const double&, const double&, const double&,
	       const double&, const double&, const double&)*/
    
  ~Predictor();

  //step simulation
  void run();

  //Accessors
  void getHidden(Vector<List<Tensor5<Sensing> >, N_MANEUVER>& v) {v = hiddenState;}
  void getHidden(const Vector<List<Tensor5<Sensing> >, N_MANEUVER>*& v) const {v = &hiddenState;}

  void getMonitor(Vector<List<Tensor5<Sensing> >, N_MANEUVER>& v) {v = monitorState;}
  const Area& getMappingArea() const {return mappingArea;}
  const Sensing& getInitialMonitorState() const {return initialMonitorState;}
  void getErrors(Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& err, const Sensing& monitorS);
  const double& getDeltaX() const {return deltaX;}
  const double& getDeltaY() const {return deltaY;}
  const double& getDeltaTheta() const {return deltaTheta;}
  const double& getDeltaV() const {return deltaV;}
  const double& getDeltaDesiredV() const {return deltaDesiredV;}
  
  /* Computes the error propagation from the hidden vehicle to the monitor, err[n] contains the error of the n-th variable, listIndex 
     is the index of the current rectangle (from 0), tensor index (i,j,k,l,m), 
     monitorSigma is the monitor maneuver hypothesis */
  void computeDiscrError(Vector<double, 4>& err, const int& listIndex,
		const int& i, const int& j, const int& k, const int& l, const int& m,
		const Maneuver& monitorSigma);
  void computeDiscrError(Vector<double, 4>& err, const Tensor5<Sensing>*,
		const int& i, const int& j, const int& k, const int& l, const int& m);

  
  /* Computes the compatibility of a specific predicted point (and specific monitor maneuver) of the state space with the measurement.
   Compatibility is defined as the sum of the squares (x - x_meas)/errX + (y - y_meas)/errY ... */
  static double getCompatibility(const Sensing& measure,
				 const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
				 const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& errV,
				 const int& listIndex,
		const int& i, const int& j, const int& k, const int& l, const int& m,
		const Maneuver& monitorSigma);
  
  
  /* compare measurement with simulation. This method computes the compatibility of 
     the hypothesis (monitor maneuver) with the actual measurement at the next simulation step.
     Compatibility is defined as the sum of the ratios (x - x_m)/errX + (y - y_m)/errY + ... 
     For each hypothesis, we take the best candidate (smaller compatibility) */
  /*double getCompatibility    */ 

  /* Compares the actual measurement with the predictions. Returns a compatibility vector, containing the 
   compatibility of each hypothesis of the hidden vehicle with the measurement */
  static void compareWithMeasure(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
				 const Vector<List<Tensor5<Vector<double,4> > >, N_MANEUVER>& errV,
				 Vector<List<Tensor5<double> >, N_MANEUVER>& compatibility);

  /* Takes the compatibility vector as input and computes the most compatible prediction (point in the state space) 
   for each monitor maneuver, which is stored into the coordinates bestList, iV, jV, ... 
  Also, stores the best compatibility value for each maneuver */
  static void chooseBestPredictions(const Vector<List<Tensor5<double> >, N_MANEUVER>& compatibility,
			    Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold);
  
  /* Checks if the most compatible prediction pass a compatibility cut. If only one does, return true, and detects a maneuver */
  bool detectManeuver(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
			     const Vector<List<Tensor5<Vector<double,4> > >, N_MANEUVER>& errV,
			     Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold);

  static List<Maneuver> projectToManeuver(const Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold);
  
};

#endif
