/*
AUTHOR = Federico Massa
DATE = 29.06.2017
DESCRIPTION = Helper class for Predictor object. Prediction begins with the measurement of the monitored vehicle's state.
				This measurement is subject to errors that propagate throughout the simulation. This class calculates the propagation of the error
				with a simple MC-like simulation

*/
#ifndef SENSOR_ERROR_SIMULATOR_H
#define SENSOR_ERROR_SIMULATOR_H

#include "PhysicalLayer.h"
#include "State.h"
#include "Vector.h"

extern int now;

enum Maneuver;
template<typename T> class List;

class SensorErrorSimulator
{

	PhysicalLayer pLayer;
	int beginPredictionTime;
	State initialState;
	// Matrix containing each prediction outcome, like 
	Vector<Vector<State, 2>, 4> predictionOutcome;

	Vector<double, 4> sensorErrors;
public:
	SensorErrorSimulator();
	void init(const State&);

	//! \brief Runs the physicalLayer of the monitored vehicle for each error hypothesis
	void predictionStep(const Maneuver&, const List<State>&);

	//! \brief Estimate differential once the prediction is over
	Vector<double, 4> estimateDifferential();

	State changeStateAtIndex(const int& varIndex, const int& signIndex, const State& iState);


};

#endif