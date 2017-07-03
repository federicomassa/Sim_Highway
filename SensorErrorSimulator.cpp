#include "SensorErrorSimulator.h"
#include "Sensing.h"
#include "List.h"
#include "systemTypes.h"
#include "utility.h"

SensorErrorSimulator::SensorErrorSimulator()
{}

void SensorErrorSimulator::init(const State& q)
{
	beginPredictionTime = now;

	initialState = q;


	// Errors initialization
	sensorErrors[0] = SIGMA_X;
	sensorErrors[1] = SIGMA_Y;
	sensorErrors[2] = fabs(RELATIVE_SIGMA_THETA * initialState.theta);
	sensorErrors[3] = fabs(RELATIVE_SIGMA_V * initialState.v);

	// Initialization of the outcome
	for (int i = 0; i < predictionOutcome.length; i++)
	{

		for (int j = 0; j < predictionOutcome[i].length; j++)
		{
			if (predictionOutcome[i].length != 2)
				error("SensorErrorSimulator::init", "Development error: vector size not 2");

			predictionOutcome[i][j] = changeStateAtIndex(i, j, initialState);

		}
	}


	if (sensorErrors[0] < 1E-10 || sensorErrors[1] < 1E-10 || sensorErrors[2] < 1E-10 || sensorErrors[3] < 1E-10)
		error("SensorErrorSimulator::init", "Errors cannot be zero");
}

void SensorErrorSimulator::predictionStep(const Maneuver& sigma, const List<State>& qList)
{
	for (int i = 0; i < predictionOutcome.length; i++)
	{
		for (int j = 0; j < predictionOutcome[i].length; j++)
		{
			pLayer.init(predictionOutcome[i][j]);
			pLayer.computeNextQ(sigma, qList);

			// update step
			predictionOutcome[i][j] = pLayer.getNextQ();
		}
	}
}

State SensorErrorSimulator::changeStateAtIndex(const int& varIndex, const int& signIndex, const State& iState)
{
	if (varIndex < 0 || varIndex > 3 || signIndex < 0 || signIndex > 1)
		error("SensorErrorSimulator::changeStateAtIndex", "indexes out of bounds");

	double sign = (signIndex == 1) ? 1.0 : -1.0;

	State changedState(iState);


	changedState[varIndex] = changedState[varIndex] + sign * sensorErrors[varIndex];

	/*
		std::cout << "Initial State: " << initialState << std::endl;
		std::cout << "Var index: " << varIndex << '\t' << sign << std::endl << ' ' << sensorErrors[varIndex];
		std::cout << "Change State: " << changedState << std::endl;
	*/


	return changedState;
}

Vector<double, 4> SensorErrorSimulator::estimateDifferential()
{
	if (sensorErrors[0] < 1E-10 || sensorErrors[1] < 1E-10 || sensorErrors[2] < 1E-10 || sensorErrors[3] < 1E-10)
		error("SensorErrorSimulator::estimateDifferential", "Errors cannot be zero");

	// central derivative to decrease error
	Vector<double, 4> result;

	for (int i = 0; i < 4; i++)
	{
		result[i] = 0.0;
	}


	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[j] = result[j] + sqrt(pow((predictionOutcome[i][1][j] - predictionOutcome[i][0][j]) / 2.0, 2));
		}
	}


	return result;
}