/*
  Implementation of class Predictor
 */

/* FIXME Fix issue with counter. When vehicles are no longer visible there are problems
FIXME when considering uniform speed error, agent qList does not change as it should
*/


#include <cmath>
#include <iostream>
#include <iomanip>

#include "Predictor.h"
#include "Environment.h"
#include "DynVector.h"
#include "systemParms.h"
#include "utility.h"

const double Predictor::compatibilityCut = 10;
const double Predictor::deltaX = VISIBLE_DISTANCE / 8;
const double Predictor::deltaY = LANE_HEIGHT / 2;
const double Predictor::deltaTheta = 2 * MAX_THETA / 3;
const double Predictor::deltaV = 1.0 / 4;
const double Predictor::deltaDesiredV = 1.0 / 4;


/* Function to compute the indexes needed for the numerical derivative calculus */
void computeIndexes(const int& i, const int& dim, int& iPlus, int& iMinus)
{
	if (i == 0)
	{
		iPlus = 1;
		iMinus = 0;
	}
	else if (i == (dim - 1))
	{
		iPlus = 0;
		iMinus = -1;
	}
	else
	{
		iPlus = 1;
		iMinus = -1;
	}
}

Predictor::Predictor(const int& aID, const int& monitorIndex, const int& nTSteps) : agentID(aID), monitorID(monitorIndex), nTimeSteps(nTSteps)
{
	if (nTSteps <= 0)
		error("Predictor", "Invalid number of time steps, should be > 0");
}

void Predictor::init(const State& aState, const Maneuver& aManeuver, const List<Sensing>& sL)
{
	iAgentState = aState;
	agentManeuver = aManeuver;
	sList = sL;
}


Predictor::~Predictor() {}

void Predictor::run(const Area& obs)
{
	/* Purge elements */
	for (int i = 0; i < N_MANEUVER; i++)
	{
		agentState[i].reset();
		monitorState[i].purge();
		discrErrors[i].purge();
		uniformErrors[i].purge();
		forwardSensorErrors[i].purge();
		hiddenState[i].purge();
	}

	/*                */

	//Temporary environment to compute agent hidden area, +1 to account for the agent
	Environment tmpEnv(sList.count() + 1, CONF.cRadius, CONF.cProb);

	//list containing all the visible vehicles
	Sensing agentS(agentID, iAgentState, iAgentState.v, agentManeuver);
	List<Sensing> tmpSList = sList;
	// now agent is the first
	tmpSList.insHead(agentS);

	List<State> tmpQList;
	Iterator<Sensing> iSensing(tmpSList);
	Sensing tmpS;

	List<Parms> tmpPList;
	State iMonitorState;
	int nCounter = 0;
	int nMonitor = -1;
	//need q and p list to initialize vehicles
	while (iSensing(tmpS))
	{
		tmpQList.insTail(tmpS.q);
		tmpPList.insTail(tmpS.q.v);
		if (tmpS.agentID == monitorID)
		{
			initialMonitorState = tmpS;
			iMonitorState = tmpS.q;
			nMonitor = nCounter;
		}
		nCounter++;
	}

	tmpEnv.initVehicles(tmpQList, tmpPList);
	Area beginningMonitorObs;
	// NB agent is the first, so index 0
	if (agentID == 0)
		std::cout << "obs: " << obs << std::endl;

	tmpEnv.observableArea(nMonitor, beginningMonitorObs);
	if (agentID == 0)
		std::cout << "monitorObs: " << beginningMonitorObs << std::endl;
	/* Hidden area is calculated based on the visibility of
	 the agent. But we want to add the part hidden to the agent
	that is visible to the monitored vehicle */

	// We map the hidden vehicle in the area visible to the monitored but invisible to the observer
	mappingArea = beginningMonitorObs - obs;
	mappingArea.simplify();
	if (agentID == 0)
		std::cout << "mappingArea: " << mappingArea << std::endl;
	/*
		// case monitored ahead
		Vector<Vector<double, 2>, 2> newRect;

		if (iAgentState.x < iMonitorState.x)
		{
			newRect[0][0] = iAgentState.x + VISIBLE_DISTANCE;
			newRect[0][1] = iMonitorState.x + VISIBLE_DISTANCE;
			newRect[1][0] = MIN_LANE;
			newRect[1][1] = MAX_LANE + 1;
		}
		else
		{
			newRect[0][0] = iMonitorState.x - VISIBLE_DISTANCE;
			newRect[0][1] = iAgentState.x - VISIBLE_DISTANCE;
			newRect[1][0] = MIN_LANE;
			newRect[1][1] = MAX_LANE + 1;
		}
		hidden->addRect(newRect);
		hidden->simplify();
	*/

	// Get list of rectangles forming the area to
	// compute x,y segmentation

	List<Rectangle> rList;

//	hidden->getRectList(rList);
	mappingArea.getRectList(rList);

	/* Add an empty rectangle to the TOP OF THE LIST (important for error computation?), that represents the hypothesis
	 with no hidden vehicles -> tensor(1,1,1,1,1) */
	Rectangle dummyRect;
	rList.insHead(dummyRect);

	if (agentID == 0)
		std::cout << agentID << " sees list size " << rList.count() << std::endl;

	// loop on the possible monitor maneuvers
	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{
		Iterator<Rectangle> iRect(rList);
		Rectangle tmpRect;

		int rCount = -1;
		while (iRect(tmpRect))
		{
			rCount++;
			bool noHiddenVehicle = false;

			if (tmpRect.isDummy)
				noHiddenVehicle = true;

			/* FIXME */
			//if (!noHiddenVehicle)
			//	continue;


			Vector<Vector<double, 2>, 2> b;
			tmpRect.getBounds(b);

			// rectangle bounds
			double xmin = b[0][0];
			double xmax = b[0][1];
			double ymin = b[1][0];
			double ymax = b[1][1];

			// Not too close to the road edges
			if (ymin < (MIN_LANE + LANE_HEIGHT / 5.0))
				ymin = 0.2;

			if (ymax > (MAX_LANE + 4.0 / 5.0 * LANE_HEIGHT))
				ymax = MAX_LANE + 4.0 / 5.0 * LANE_HEIGHT;

			if ((xmax <= xmin || ymax <= ymin) && !noHiddenVehicle)
			{
				std::cout << "Predictor::Run, xmax < xmin?" << std::endl;
				continue;
			}

			// n bins
			int nX, nY, nTheta, nV, nDesiredV;

			// if the ratio is not exact, we also take the smaller remainder : x_____x_____x_____x___x
			if (!noHiddenVehicle)
			{
				// x, y are in the center of the cells, the others are like in the drawing
				nX = ceil((xmax - xmin) / deltaX);
				nY = ceil((ymax - ymin) / deltaY);
				nTheta = ceil(2 * MAX_THETA / deltaTheta) + 1;
				nV = ceil(1.0 / deltaV) + 1;
				nDesiredV = ceil(1.0 / deltaDesiredV) + 1;
			}
			else
			{
				nX = 1;
				nY = 1;
				nTheta = 1;
				nV = 1;
				nDesiredV = 1;
			}

			Tensor5<Sensing> tmpHidden(nX, nY, nTheta, nV, nDesiredV);
			Tensor5<Sensing> tmpMonitor(nX, nY, nTheta, nV, nDesiredV);
			Tensor5<Vector<double, 4> > uniformTens(nX, nY, nTheta, nV, nDesiredV);
			Tensor5<Vector<double, 4> > sensorErrorTens(nX, nY, nTheta, nV, nDesiredV);


			// Hypothesis on a grid
			for (int xi = 0; xi < nX; xi++)
			{	for (int yi = 0; yi < nY; yi++)
				{	for (int thetai = 0; thetai < nTheta; thetai++)
					{	for (int vi = 0; vi < nV; vi++)
							for (int desiredVi = 0; desiredVi < nDesiredV; desiredVi++)
							{

								//Simulated environment with all vehicles, including the one hidden
								Environment* simulEnv;

								if (!noHiddenVehicle)
									simulEnv = new Environment(sList.count() + 2, CONF.cRadius, CONF.cProb);
								else
									simulEnv = new Environment(sList.count() + 1, CONF.cRadius, CONF.cProb);

								//check for very small rect case
								// compute state value taking the center of each bin
								double x, y, theta, v, desiredV;

								// WARNING: If mapping is changed, also Image::addAreaWithHidden has to be changed and hiddenCells construction in ReputationManager
								if (xi != nX - 1)
									x = xmin + deltaX / 2.0 + xi * deltaX;
								else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
									x = (xmin + (nX - 1) * deltaX + xmax) / 2.0;

								if (yi != nY - 1)
									y = ymin + deltaY / 2.0 + yi * deltaY;
								else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
									y = (ymin + (nY - 1) * deltaY + ymax) / 2.0;

								if (thetai != nTheta - 1)
									theta = -MAX_THETA + thetai * deltaTheta;
								else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
									theta = MAX_THETA;

								if (vi != nV - 1)
									v = 0 + vi * deltaV;
								else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
									v = 1.0;

								if (desiredVi != nDesiredV - 1)
									desiredV = 0 + desiredVi * deltaDesiredV;
								else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
									desiredV = 1.0;


								/* If in the hypothesis with no hidden vehicles, fill but it won't be used */
								State hiddenQ(x, y, theta, v, desiredV, "FAST");

								// Simulated sensor response, hidden vehicle as special ID = 100
								Sensing hiddenS(100, hiddenQ, hiddenQ.v, FAST);
								Parms hiddenP = hiddenQ.v;
								// fill tensor
								tmpHidden(xi, yi, thetai, vi, desiredVi) = hiddenS;

								//Simulate the environment
								if (!noHiddenVehicle)
								{
									tmpSList.insTail(hiddenS);
									tmpQList.insTail(hiddenQ);
									tmpPList.insTail(hiddenP);
								}

								simulEnv->initVehicles(tmpQList, tmpPList);
								iSensing.initialize(tmpSList);

								for (int n = 0; n < simulEnv->getNVehicles(); n++)
								{
									if (iSensing(tmpS))
										simulEnv->getVehicles()[n].setID(tmpS.agentID);
									else
										error("Predictor", "Something wrong in sensing list");
								}

								//simulEnv.run();

								Vehicle* monitorV = NULL;
								int monitorIndex = -1;
								for (int n = 0; n < simulEnv->getNVehicles(); n++)
								{
									if (simulEnv->getVehicles()[n].getID() == monitorID)
									{
										monitorV = &(simulEnv->getVehicles()[n]);
										monitorIndex = n;
										break;
									}
								}
								if (monitorV == NULL)
									error("Predictor", "Monitor not found in environment");

								State monitorQ = monitorV->getQ();
								//	std::cout << "MonitorV: " << monitorQ << std::endl;
								/* Set monitor maneuver on separate physical layer. We do not
								   use vehicle directly (preRun) because we do not want
								   to run the automaton --> NB simulEnv for monitored and agent vehicle
								   is not updated with the evolved state */
								monitorQ.initManeuver = maneuverToStr((Maneuver)sigma);
								monitorPLayer.init(monitorQ, monitorQ.v);


								// We will evolve in a simplified way all vehicles except the agent (we know a better model for it) and monitored (specific sigma), hence NVeh - 2
								DynVector<Vector<PhysicalLayer, 2> > monitorPLayerError(simulEnv->getNVehicles() - 2);

								for (int cnt = 0; cnt < monitorPLayerError.length; cnt++)
								{
									monitorPLayerError[cnt][0].init(monitorQ, monitorQ.v);
									monitorPLayerError[cnt][1].init(monitorQ, monitorQ.v);
								}

								agentPLayer.init(iAgentState, iAgentState.v);
								/* Predict each vehicle's behaviour within nTSteps time steps.
								   Hypothesis: all vehicles except observer and monitored move with const speed */

								/* Vector containing extreme cases to compute errors due to uniform speed motion
								   hypothesis. External vector is for vehicles (except agent and monitored), internal is
								   for the number of cases sampled (3 in this case: const acc, 0 acc, -const acc) */
								DynVector<Vector<State, 2> > extremeCaseStates;
								/* Evaluate monitored vehicle's observable area */
								Area iMonitorObs;
								simulEnv->observableArea(monitorIndex, iMonitorObs);

								//Instantiate MC simulator for sensor errors propagation
								sensorSimul.init(monitorV->getQ());

								for (int steps = 0; steps < nTimeSteps; steps++)
								{
									//create monitor qList
									List<State> monitorQList;
									List<State> agentQList;

									/* Evaluate monitored vehicle's observable area */
									Area monitorObs;
									simulEnv->observableArea(monitorIndex, monitorObs);

									// MC sensor error simulator step
									sensorSimul.predictionStep((Maneuver)sigma, monitorQList);

									int counter = 0;
									for (int n = 0; n < simulEnv->getNVehicles(); n++)
									{
										if (simulEnv->getVehicles()[n].getID() != monitorID && simulEnv->getVehicles()[n].inArea(monitorObs))
											monitorQList.insHead(simulEnv->getVehicles()[n].getQ());

										if (simulEnv->getVehicles()[n].getID() != agentID && simulEnv->getVehicles()[n].inArea(obs))
											agentQList.insHead(simulEnv->getVehicles()[n].getQ());

										if (simulEnv->getVehicles()[n].getID() == monitorID ||
										        simulEnv->getVehicles()[n].getID() == agentID)
											continue;

										if (steps == 0)
										{
											if (extremeCaseStates.length == 0)
												extremeCaseStates.init(monitorPLayerError.length);

											extremeCaseStates[counter][0] = simulEnv->getVehicles()[n].getQ();
											extremeCaseStates[counter][1] = extremeCaseStates[counter][0];
										}

										counter++;
									}

									// Dynamic prediction step
									//monitorPLayer.updateQ();
									monitorPLayer.computeNextQ((Maneuver)sigma, monitorQList, false);
									//std::cout << "Step: " << steps << '\t' << "Monitor NextQ: " << monitorPLayer.getNextQ() << std::endl;
									agentPLayer.computeNextQ((Maneuver)agentManeuver, agentQList);

									/* Same thing for the extreme case, to compute the uniform speed
									   hypothesis error */

									/* NB This works because counter is defined as below, where
									   extremeCaseStates is filled. No other reference exists
									   between vehicles in simulEnv and extremeCaseStates... */
									for (int cnt = 0; cnt < monitorPLayerError.length; cnt++)
									{
										for (int extr = 0; extr < 2; extr++)
										{
											List<State> monitorQListError;
											Vehicle tmpV;

											tmpV.init(extremeCaseStates[cnt][extr], extremeCaseStates[cnt][extr].v);

											if (tmpV.inArea(monitorObs))
												monitorQListError.insHead(extremeCaseStates[cnt][extr]);

											int counter2 = 0;

											for (int n = 0; n < simulEnv->getNVehicles(); n++)
											{


												if (simulEnv->getVehicles()[n].getID() == agentID)
												{
													monitorQListError.insHead(simulEnv->getVehicles()[n].getQ());
													continue;
												}

												if (simulEnv->getVehicles()[n].getID() == monitorID)
													continue;

												if (cnt != counter2 && simulEnv->getVehicles()[n].inArea(monitorObs))
													monitorQListError.insHead(simulEnv->getVehicles()[n].getQ());


												counter2++;

											}


											/*				if (monitorQListError.count() == 1 && !tmpRect.isDummy)
															std::cout << "monitorQListError is 1 at: " << steps << std::endl;*/
											monitorPLayerError[cnt][extr].computeNextQ((Maneuver)sigma, monitorQListError, false);
											monitorPLayerError[cnt][extr].updateQ();

										}
									}

									/* update pLayer for next iteration */
									monitorPLayer.updateQ();
									agentPLayer.updateQ();

									/* evolve remaining vehicles */

									/* Also, estimate errors coming from the uniform speed hypothesis:
									   this is estimated by measuring x,y,... of the monitored vehicle
									   when the others move with a const acceleration/deceleration motion*/

									counter = 0;

									for (int n = 0; n < simulEnv->getNVehicles(); n++)
									{
										if (simulEnv->getVehicles()[n].getID() == agentID)
										{
											Vehicle& veh = simulEnv->getVehicles()[n];
											veh.setQ(agentPLayer.getQ());
											continue;
										}

										if (simulEnv->getVehicles()[n].getID() == monitorID)
										{
											Vehicle& veh = simulEnv->getVehicles()[n];
											veh.setQ(monitorPLayer.getQ());
											continue;
										}



										Vehicle& veh = simulEnv->getVehicles()[n];
										State oldQ = veh.getQ();
										/* uniform speed motion */
										State newQ(oldQ.x + oldQ.v * cos(oldQ.theta),
										           oldQ.y + oldQ.v * sin(oldQ.theta),
										           oldQ.theta,
										           oldQ.v,
										           oldQ.desiredV);

										veh.setQ(newQ);


										/* Extreme cases computation */

										/* Acceleration */
										if ((extremeCaseStates[counter][0].v + ERROR_ACC) < /*extremeCaseStates[counter][0].desiredV/MAX_SPEED*/ MAX_SPEED)
											extremeCaseStates[counter][0] = State(extremeCaseStates[counter][0].x +
											                                      extremeCaseStates[counter][0].v * cos(extremeCaseStates[counter][0].theta) +
											                                      0.5 * cos(extremeCaseStates[counter][0].theta) * ERROR_ACC,
											                                      extremeCaseStates[counter][0].y +
											                                      extremeCaseStates[counter][0].v * sin(extremeCaseStates[counter][0].theta) +
											                                      0.5 * sin(extremeCaseStates[counter][0].theta) * ERROR_ACC,
											                                      extremeCaseStates[counter][0].theta,
											                                      extremeCaseStates[counter][0].v + ERROR_ACC,
											                                      extremeCaseStates[counter][0].desiredV);
										else
											extremeCaseStates[counter][0] = State(extremeCaseStates[counter][0].x +
											                                      extremeCaseStates[counter][0].v * cos(extremeCaseStates[counter][0].theta) +
											                                      0.5 * cos(extremeCaseStates[counter][0].theta) * (1.0 - extremeCaseStates[counter][0].v),
											                                      extremeCaseStates[counter][0].y +
											                                      extremeCaseStates[counter][0].v * sin(extremeCaseStates[counter][0].theta) +
											                                      0.5 * sin(extremeCaseStates[counter][0].theta) * (1.0 - extremeCaseStates[counter][0].v),
											                                      extremeCaseStates[counter][0].theta,
											                                      1.0,
											                                      extremeCaseStates[counter][0].desiredV);


										/* Deceleration */
										if ((extremeCaseStates[counter][1].v - ERROR_ACC) > 0)
											extremeCaseStates[counter][1] = State(extremeCaseStates[counter][1].x +
											                                      extremeCaseStates[counter][1].v * cos(extremeCaseStates[counter][1].theta) -
											                                      0.5 * cos(extremeCaseStates[counter][1].theta) * ERROR_ACC,
											                                      extremeCaseStates[counter][1].y +
											                                      extremeCaseStates[counter][1].v * sin(extremeCaseStates[counter][1].theta) -
											                                      0.5 * sin(extremeCaseStates[counter][1].theta) * ERROR_ACC,
											                                      extremeCaseStates[counter][1].theta,
											                                      extremeCaseStates[counter][1].v - ERROR_ACC,
											                                      extremeCaseStates[counter][1].desiredV);
										else
											extremeCaseStates[counter][1] = State(extremeCaseStates[counter][1].x +
											                                      extremeCaseStates[counter][1].v * cos(extremeCaseStates[counter][1].theta) +
											                                      0.5 * cos(extremeCaseStates[counter][1].theta) * (0.0 - extremeCaseStates[counter][1].v),
											                                      extremeCaseStates[counter][1].y +
											                                      extremeCaseStates[counter][1].v * sin(extremeCaseStates[counter][1].theta) +
											                                      0.5 * sin(extremeCaseStates[counter][1].theta) * (0.0 - extremeCaseStates[counter][1].v),
											                                      extremeCaseStates[counter][1].theta,
											                                      0.0,
											                                      extremeCaseStates[counter][1].desiredV);



										// if (counter == 0)
										// 	{
										// 	  std::cout << "Diff: " << std::fixed << std::setprecision(6) <<
										// 	    extremeCaseStates[counter][1].v - simulEnv->getVehicles()[n].getQ().v << std::endl;
										// 	}

										counter++;

									}

								}


								State monitorNextQ = monitorPLayer.getNextQ();
								Sensing monitorS(monitorID, monitorNextQ, monitorNextQ.v, (Maneuver)sigma);

								tmpMonitor(xi, yi, thetai, vi, desiredVi) = monitorS;

								//remove for next iteration
								if (!noHiddenVehicle)
								{
									tmpSList.extrTail(hiddenS);
									tmpQList.extrTail(hiddenQ);
									tmpPList.extrTail(hiddenP);
								}

								// Now compute errors due to initial sensor inaccuracy
								sensorErrorTens(xi, yi, thetai, vi, desiredVi) = sensorSimul.estimateDifferential();
								//sensorErrorTens(xi, yi, thetai, vi, desiredVi) = Vector<double, 4>();

								/* Now compute errors due to non uniform speed */
								int counter = 0;

								Vector<double, 4> uniformVect;

								for (int n = 0; n < 4; n++)
									uniformVect[n] = 0;

								for (int n = 0; n < simulEnv->getNVehicles(); n++)
								{
									if (simulEnv->getVehicles()[n].getID() == agentID ||
									        simulEnv->getVehicles()[n].getID() == monitorID)
										continue;

									/* divide by 12: prior hypothesis on uniformity */
									uniformVect[0] += (pow(monitorPLayerError[counter][0].getQ().x - monitorNextQ.x, 2) +
									                   pow(monitorPLayerError[counter][1].getQ().x - monitorNextQ.x, 2)) / 12.0;
									uniformVect[1] += (pow(monitorPLayerError[counter][0].getQ().y - monitorNextQ.y, 2) +
									                   pow(monitorPLayerError[counter][1].getQ().y - monitorNextQ.y, 2) ) / 12.0;
									uniformVect[2] += (pow(monitorPLayerError[counter][0].getQ().theta - monitorNextQ.theta, 2) +
									                   pow(monitorPLayerError[counter][1].getQ().theta - monitorNextQ.theta, 2) ) / 12.0;
									uniformVect[3] += (pow(monitorPLayerError[counter][0].getQ().v - monitorNextQ.v, 2) +
									                   pow(monitorPLayerError[counter][1].getQ().v - monitorNextQ.v, 2) ) / 12.0;


									counter++;
								}

								for (int n = 0; n < 4; n++)
									uniformVect[n] = sqrt(uniformVect[n]);

								uniformTens(xi, yi, thetai, vi, desiredVi) = uniformVect;

								delete simulEnv;
							}
					}
				}
			}


			// WARNING hiddenState is filled also in the noHiddenVehicle case for list size consistency
			// remember that the first element of the list corresponds to this case
			hiddenState[sigma].insTail(tmpHidden);

			monitorState[sigma].insTail(tmpMonitor);
			uniformErrors[sigma].insTail(uniformTens);
			forwardSensorErrors[sigma].insTail(sensorErrorTens);
		}


		/* Now compute discretization errors */
		List<Tensor5<Vector<double, 4> > > errList;

		for (int list = 0; list < monitorState[sigma].count(); list++)
		{

			bool noHiddenVehicle = false;

			const Tensor5<Sensing>* monitor;
			monitorState[sigma].getElem(monitor, list);

			Tensor5<Vector<double, 4> > errTens(monitor->Dim1, monitor->Dim2, monitor->Dim3, monitor->Dim4, monitor->Dim5);

			/* When there is a hidden vehicle, nX... is at least 2 */
			if (monitor->Dim1 == 1 && monitor->Dim2 == 1 && monitor->Dim3 == 1 && monitor->Dim4 == 1 && monitor->Dim5 == 1)
				noHiddenVehicle = true;

			for (int i = 0; i < monitor->Dim1; i++)
				for (int j = 0; j < monitor->Dim2; j++)
					for (int k = 0; k < monitor->Dim3; k++)
						for (int l = 0; l < monitor->Dim4; l++)
							for (int m = 0; m < monitor->Dim5; m++)
							{
								Vector<double, 4> tmpErr;

								if (!noHiddenVehicle)
									computeDiscrError(tmpErr, monitor, i, j, k, l, m);
								else
								{
									tmpErr[0] = 0;
									tmpErr[1] = 0;
									tmpErr[2] = 0;
									tmpErr[3] = 0;
								}

								errTens(i, j, k, l, m) = tmpErr;

								/*		      if ((Maneuver)sigma == PLATOON)
								if (i == 1 && j == 0 && k == 0 && l == 5 && m == 8)
								  {
								    for (int c = 0; c < 4; c++)
								      std::cout << "Errs: " << tmpErr[c] << '\t';
								    std::cout << std::endl;
								    }*/


							}

			errList.insTail(errTens);

		}

		discrErrors[sigma] = errList;
	}
}

void Predictor::computeDiscrError(Vector<double, 4>& err, const int& listIndex,
                                  const int& i, const int& j, const int& k, const int& l, const int& m,
                                  const Maneuver& monitorSigma)
{
	const Tensor5<Sensing>* monitor;

	monitorState[(int)monitorSigma].getElem(monitor, listIndex);

	computeDiscrError(err, monitor, i, j, k, l, m);
}

void Predictor::computeDiscrError(Vector<double, 4>& err, const Tensor5<Sensing>* monitor,
                                  const int& i, const int& j, const int& k, const int& l, const int& m)
{

	/* From point (i,j,k,l,m) move to the neighborhood */
	/*
	for (int a = -1; a <= 1; a++)
	  for (int b = -1; b <= 1; b++)
	    for (int c = -1; c <= 1; c++)
	for (int d = -1; d <= 1; d++)
	  for (int e = -1; e <= 1; e++)
	    {
	      if (a != 0 && b != 0 && c != 0 && d != 0 && e != 0 &&
		  i+a < monitor->Dim1 && i+a >= 0 &&
		  j+b < monitor->Dim2 && j+b >= 0 &&
		  k+c < monitor->Dim3 && k+c >= 0 &&
		  l+d < monitor->Dim4 && l+d >= 0 &&
		  m+e < monitor->Dim5 && m+e >= 0
		  )
		points.insTail((*monitor)(i+a, j+b, k+c, l+d, m+e));
	    }
	*/

	/* Normally compute central derivative, on the edges compute side derivative */

	int iPlus, iMinus, jPlus, jMinus, kPlus, kMinus, lPlus, lMinus, mPlus, mMinus;

	computeIndexes(i, monitor->Dim1, iPlus, iMinus);
	computeIndexes(j, monitor->Dim2, jPlus, jMinus);
	computeIndexes(k, monitor->Dim3, kPlus, kMinus);
	computeIndexes(l, monitor->Dim4, lPlus, lMinus);
	computeIndexes(m, monitor->Dim5, mPlus, mMinus);

	double errX = 0, errY = 0, errTheta = 0, errV = 0;

	/* sum of partial derivatives/12 because the error is uniformly distributed */
	if (monitor->Dim1 > 1 && monitor->Dim2 > 1)
	{
		errX += pow(((*monitor)(i + iPlus, j, k, l, m).q.x - (*monitor)(i + iMinus, j, k, l, m).q.x) / double(iPlus - iMinus), 2);
		errX += pow(((*monitor)(i, j + jPlus, k, l, m).q.x - (*monitor)(i, j + jMinus, k, l, m).q.x) / double(jPlus - jMinus), 2);
		errX += pow(((*monitor)(i, j, k + kPlus, l, m).q.x - (*monitor)(i, j, k + kMinus, l, m).q.x) / double(kPlus - kMinus), 2);
		errX += pow(((*monitor)(i, j, k, l + lPlus, m).q.x - (*monitor)(i, j, k, l + lMinus, m).q.x) / double(lPlus - lMinus), 2);
		errX += pow(((*monitor)(i, j, k, l, m + mPlus).q.x - (*monitor)(i, j, k, l, m + mMinus).q.x) / double(mPlus - mMinus), 2);

		errY += pow(((*monitor)(i + iPlus, j, k, l, m).q.y - (*monitor)(i + iMinus, j, k, l, m).q.y) / double(iPlus - iMinus), 2);
		errY += pow(((*monitor)(i, j + jPlus, k, l, m).q.y - (*monitor)(i, j + jMinus, k, l, m).q.y) / double(jPlus - jMinus), 2);
		errY += pow(((*monitor)(i, j, k + kPlus, l, m).q.y - (*monitor)(i, j, k + kMinus, l, m).q.y) / double(kPlus - kMinus), 2);
		errY += pow(((*monitor)(i, j, k, l + lPlus, m).q.y - (*monitor)(i, j, k, l + lMinus, m).q.y) / double(lPlus - lMinus), 2);
		errY += pow(((*monitor)(i, j, k, l, m + mPlus).q.y - (*monitor)(i, j, k, l, m + mMinus).q.y) / double(mPlus - mMinus), 2);

		errTheta += pow(((*monitor)(i + iPlus, j, k, l, m).q.theta - (*monitor)(i + iMinus, j, k, l, m).q.theta) / double(iPlus - iMinus), 2);
		errTheta += pow(((*monitor)(i, j + jPlus, k, l, m).q.theta - (*monitor)(i, j + jMinus, k, l, m).q.theta) / double(jPlus - jMinus), 2);
		errTheta += pow(((*monitor)(i, j, k + kPlus, l, m).q.theta - (*monitor)(i, j, k + kMinus, l, m).q.theta) / double(kPlus - kMinus), 2);
		errTheta += pow(((*monitor)(i, j, k, l + lPlus, m).q.theta - (*monitor)(i, j, k, l + lMinus, m).q.theta) / double(lPlus - lMinus), 2);
		errTheta += pow(((*monitor)(i, j, k, l, m + mPlus).q.theta - (*monitor)(i, j, k, l, m + mMinus).q.theta) / double(mPlus - mMinus), 2);

		errV += pow(((*monitor)(i + iPlus, j, k, l, m).q.v - (*monitor)(i + iMinus, j, k, l, m).q.v) / double(iPlus - iMinus), 2);
		errV += pow(((*monitor)(i, j + jPlus, k, l, m).q.v - (*monitor)(i, j + jMinus, k, l, m).q.v) / double(jPlus - jMinus), 2);
		errV += pow(((*monitor)(i, j, k + kPlus, l, m).q.v - (*monitor)(i, j, k + kMinus, l, m).q.v) / double(kPlus - kMinus), 2);
		errV += pow(((*monitor)(i, j, k, l + lPlus, m).q.v - (*monitor)(i, j, k, l + lMinus, m).q.v) / double(lPlus - lMinus), 2);
		errV += pow(((*monitor)(i, j, k, l, m + mPlus).q.v - (*monitor)(i, j, k, l, m + mMinus).q.v) / double(mPlus - mMinus), 2);
	}

	err[0] = sqrt(errX / 12.0);
	err[1] = sqrt(errY / 12.0);
	err[2] = sqrt(errTheta / 12.0);
	err[3] = sqrt(errV / 12.0);


}


double Predictor::getCompatibility(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
                                   const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& errV,
                                   const int& listIndex,
                                   const int& i, const int& j, const int& k, const int& l, const int& m,
                                   const Maneuver& monitorSigma)
{
	/* Get tensor from the list */
	const Tensor5<Sensing>* monitor;
	monitorPrediction[monitorSigma].getElem(monitor, listIndex);


	/* error handling */
	if (i < 0 || i >= monitor->Dim1 ||
	        j < 0 || j >= monitor->Dim2 ||
	        k < 0 || k >= monitor->Dim3 ||
	        l < 0 || l >= monitor->Dim4 ||
	        m < 0 || m >= monitor->Dim5)
		error("Predictor::getCompatibility", "Invalid index");

	/* Get error relative to that point */
	const Tensor5<Vector<double, 4> >* errTens;
	errV[monitorSigma].getElem(errTens, listIndex);
	Vector<double, 4> err = (*errTens)(i, j, k, l, m);


	double compValue = 0;

	/* Definition of compValue of a point with the measurement: sqrt of sum of squares of (x_i - x_meas)/errX + ... */

	/* Special case: if error is 0 and the distance from the measurement is != 0 => not compatible (large number) */
	if (err[0] < 1E-10)
	{

		if (fabs((*monitor)(i, j, k, l, m).q.x - measure.q.x) > 1E-10)
		{
			return 1000000;
		}
		/*      else
		    std::cout << "DiffX: " << fabs((*monitor)(i,j,k,l,m).q.x - measure.q.x) << std::endl;*/
	}
	else
		compValue += pow(((*monitor)(i, j, k, l, m).q.x - measure.q.x) / err[0], 2);

	if (err[1] < 1E-10)
	{

		if (fabs((*monitor)(i, j, k, l, m).q.y - measure.q.y) > 1E-10)
			return 1000000;
		/*      else
		    std::cout << "DiffY: " << fabs((*monitor)(i,j,k,l,m).q.y - measure.q.y) << std::endl;*/
	}
	else
		compValue += pow(((*monitor)(i, j, k, l, m).q.y - measure.q.y) / err[1], 2);


	if (err[2] < 1E-10)
	{

		if (fabs((*monitor)(i, j, k, l, m).q.theta - measure.q.theta) > 1E-10)
			return 1000000;
		/*      else
		    std::cout << "DiffT: " << fabs((*monitor)(i,j,k,l,m).q.theta - measure.q.theta) << std::endl;*/
	}
	else
		compValue += pow(((*monitor)(i, j, k, l, m).q.theta - measure.q.theta) / err[2], 2);

	if (err[3] < 1E-10)
	{
		if (fabs((*monitor)(i, j, k, l, m).q.v - measure.q.v) > 1E-10)
			return 1000000;
		/*      else
		    std::cout << "DiffV: " << fabs((*monitor)(i,j,k,l,m).q.v - measure.q.v) << std::endl;*/
	}
	else
		compValue += pow(((*monitor)(i, j, k, l, m).q.v - measure.q.v) / err[3], 2);


	compValue = sqrt(compValue);

	if (monitorSigma == FAST && compValue > 10)
	{
		std::cout << ((*monitor)(i, j, k, l, m).q.x - measure.q.x) << '\t' <<
		          ((*monitor)(i, j, k, l, m).q.y - measure.q.y) << '\t' <<
		          ((*monitor)(i, j, k, l, m).q.theta - measure.q.theta) << '\t' <<
		          ((*monitor)(i, j, k, l, m).q.v - measure.q.v) << '\t' << std::endl;

		std::cout << "Err: " << err[0] << '\t' << err[1] << '\t' << err[2] << '\t' << err[3] << std::endl;

	}

	return compValue;


}


void Predictor::compareWithMeasure(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
                                   const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& errV,
                                   Vector<List<Tensor5<double> >, N_MANEUVER>& compatibilityV)
{

	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{
		/* compatibility list */
		List<Tensor5<double> > compList;

		for (int list = 0; list < monitorPrediction[sigma].count(); list++)
		{
			const Tensor5<Sensing>* monitor;
			monitorPrediction[sigma].getElem(monitor, list);

			Tensor5<double> compTens(monitor->Dim1, monitor->Dim2, monitor->Dim3, monitor->Dim4, monitor->Dim5);

			for (int i = 0; i < monitor->Dim1; i++)
				for (int j = 0; j < monitor->Dim2; j++)
					for (int k = 0; k < monitor->Dim3; k++)
						for (int l = 0; l < monitor->Dim4; l++)
							for (int m = 0; m < monitor->Dim5; m++)
							{
								compTens(i, j, k, l, m) = getCompatibility(measure, monitorPrediction, errV, list, i, j, k, l, m, (Maneuver)sigma);
							}

			compList.insTail(compTens);
		}

		compatibilityV[sigma] = compList;

	}

}

// In this function we implement what in the paper is the epsilon cut: sigma_aposteriori = projector(L bowtie_epsilon q_bar)
/* the output, compatbilityUnderThreshold, contains 1s on each entry that passed the cut, 0s on each entry that didn't. */
void Predictor::chooseBestPredictions(const Vector<List<Tensor5<double> >, N_MANEUVER>& compatibilityV,
                                      Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold)
{
	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{
		// Purge list before filling
		compatibilityUnderThreshold[sigma].purge();

		for (int list = 0; list < compatibilityV[sigma].count(); list++)
		{
			const Tensor5<double>* comp;
			compatibilityV[sigma].getElem(comp, list);

			compatibilityUnderThreshold[sigma].insTail(Tensor5<bool>(comp->Dim1, comp->Dim2, comp->Dim3, comp->Dim4, comp->Dim5));


			Tensor5<bool>* compUnderThreshold;
			compatibilityUnderThreshold[sigma].getElem(compUnderThreshold, list);

			for (int i = 0; i < comp->Dim1; i++)
				for (int j = 0; j < comp->Dim2; j++)
					for (int k = 0; k < comp->Dim3; k++)
						for (int l = 0; l < comp->Dim4; l++)
							for (int m = 0; m < comp->Dim5; m++)
							{
								if ((*comp)(i, j, k, l, m) < compatibilityCut)
								{
									(*compUnderThreshold)(i, j, k, l, m) = true;
								}
								else
									(*compUnderThreshold)(i, j, k, l, m) = false;
							}

		}

	}

}

// Total error is the sum of uniform speed and discrete grid error and sensor error
// Sensor errors are not calculated properly. The sensor error at prediction time is difficult to forward propagate. It would need MC simulation
// This leads to some inconsistencies and sometimes unexpected results (i.e. undetected maneuvers), because we use a chi2 variable that
// assumes that all variables are independent.
void Predictor::getErrors(Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& v, const Sensing& monitorS)
{
	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{
		v[sigma].purge();

		Tensor5<Sensing>* currMonitorState;

		if (discrErrors[sigma].count() != uniformErrors[sigma].count() ||
		        discrErrors[sigma].count() != forwardSensorErrors[sigma].count())
			error("Predictor::getTotalError", "List size mismatch at sigma " +
			      maneuverToStr((Maneuver)sigma) + ": " +
			      toString(discrErrors[sigma].count()) + " --- " +
			      toString(uniformErrors[sigma].count()) + " --- " +
			      toString(forwardSensorErrors[sigma].count()));

		for (int n = 0; n < discrErrors[sigma].count(); n++)
		{

			const Tensor5<Vector<double, 4> >* discrTens;
			discrErrors[sigma].getElem(discrTens, n);

			const Tensor5<Vector<double, 4> >* uniformTens;
			uniformErrors[sigma].getElem(uniformTens, n);

			const Tensor5<Vector<double, 4> >* sensorErrTens;
			forwardSensorErrors[sigma].getElem(sensorErrTens, n);


			monitorState[sigma].getElem(currMonitorState, n);


			if ((*discrTens).Dim1 != (*uniformTens).Dim1 ||
			        (*discrTens).Dim2 != (*uniformTens).Dim2 ||
			        (*discrTens).Dim3 != (*uniformTens).Dim3 ||
			        (*discrTens).Dim4 != (*uniformTens).Dim4 ||
			        (*discrTens).Dim5 != (*uniformTens).Dim5 ||
			        (*discrTens).Dim1 != (*sensorErrTens).Dim1 ||
			        (*discrTens).Dim2 != (*sensorErrTens).Dim2 ||
			        (*discrTens).Dim3 != (*sensorErrTens).Dim3 ||
			        (*discrTens).Dim4 != (*sensorErrTens).Dim4 ||
			        (*discrTens).Dim5 != (*sensorErrTens).Dim5)
				error("Predictor::getTotalError", std::string("Uniform, discrete, sensor errors size mismatch: ") +
				      '(' + toString(discrTens->Dim1) + ", " + toString(discrTens->Dim2) + ", " + toString(discrTens->Dim3) + ", " +
				      toString(discrTens->Dim4) + ", " + toString(discrTens->Dim5) + ") --- " +
				      '(' + toString(uniformTens->Dim1) + ", " + toString(uniformTens->Dim2) + ", " + toString(uniformTens->Dim3) + ", " +
				      toString(uniformTens->Dim4) + ", " + toString(uniformTens->Dim5) + ") --- " +
				      '(' + toString(sensorErrTens->Dim1) + ", " + toString(sensorErrTens->Dim2) + ", " + toString(sensorErrTens->Dim3) + ", " +
				      toString(sensorErrTens->Dim4) + ", " + toString(sensorErrTens->Dim5) + ')');

			Tensor5<Vector<double, 4> > tmpTens(discrTens->Dim1, discrTens->Dim2, discrTens->Dim3, discrTens->Dim4, discrTens->Dim5);

			for (int xi = 0; xi < discrTens->Dim1; xi++)
				for (int yi = 0; yi < discrTens->Dim2; yi++)
					for (int thetai = 0; thetai < discrTens->Dim3; thetai++)
						for (int vi = 0; vi < discrTens->Dim4; vi++)
							for (int desiredVi = 0; desiredVi < discrTens->Dim5; desiredVi++)
								for (int var = 0; var < 4; var++)
								{
									double varErr;
									switch (var)
									{
									case 0:
									{
										varErr = SIGMA_X;
										break;
									}
									case 1:
									{
										varErr = SIGMA_Y;
										break;
									}
									case 2:
									{
										varErr = RELATIVE_SIGMA_THETA * /*(*currMonitorState)(xi, yi, thetai, vi, desiredVi).q.theta*/ monitorS.q.theta;
										break;
									}
									case 3:
									{
										varErr = RELATIVE_SIGMA_V * /*(*currMonitorState)(xi, yi, thetai, vi, desiredVi).q.v*/ monitorS.q.v;
										break;
									}
									default:
										break;
									}

									// there are two sensor errors: one is the propagation of the prediction, the other is the raw measurement
									tmpTens(xi, yi, thetai, vi, desiredVi)[var] = sqrt(pow((*discrTens)(xi, yi, thetai, vi, desiredVi)[var], 2) +
									        pow((*uniformTens)(xi, yi, thetai, vi, desiredVi)[var], 2) + pow(varErr, 2) + pow((*sensorErrTens)(xi, yi, thetai, vi, desiredVi)[var], 2));

								}


			v[sigma].insTail(tmpTens);

		}
	}
}

// compatibilityUnderThreshold will be overwritten (in chooseBestPredictions)
bool Predictor::detectManeuver(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
                               const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& errV,
                               Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold)
{
	compareWithMeasure(measure, monitorPrediction, errV, compatibility);

	// compatibilityUnderThreshold is purged inside method
	chooseBestPredictions(compatibility, compatibilityUnderThreshold);


	bool returnVal = false;


	monitorLog.s << "Measure: " << measure.q << EndLine();

	// if at least one hypothesis is left, return true
	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{
		for (int list = 0; list < compatibilityUnderThreshold[sigma].count(); list++)
		{
			const Tensor5<bool>* compUT;
			compatibilityUnderThreshold[sigma].getElem(compUT, list);

			const Tensor5<double>* comp;
			compatibility[sigma].getElem(comp, list);

			const Tensor5<Sensing>* pred;
			monitorPrediction[sigma].getElem(pred, list);

			const Tensor5<Vector<double, 4> >* currErr;
			errV[sigma].getElem(currErr, list);

			for (int i = 0; i < compUT->Dim1; i++)
				for (int j = 0; j < compUT->Dim2; j++)
					for (int k = 0; k < compUT->Dim3; k++)
						for (int l = 0; l < compUT->Dim4; l++)
							for (int m = 0; m < compUT->Dim5; m++)
							{
								if ((*compUT)(i, j, k, l, m) == true)
								{
									returnVal = true;
									monitorLog.s << (Maneuver)sigma << '\t' << (*comp)(i, j, k, l, m) << '\t' << (*pred)(i, j, k, l, m).q << EndLine();
									monitorLog.s << "Errors: " << "X = " << (*currErr)(i, j, k, l, m)[0] << " Y = " << (*currErr)(i, j, k, l, m)[1] << " THETA = " << (*currErr)(i, j, k, l, m)[2] << " V = " << (*currErr)(i, j, k, l, m)[3] << EndLine();
								}
							}


		}
	}

	return returnVal;
}

List<Maneuver> Predictor::projectToManeuver(const Vector<List<Tensor5<bool> >, N_MANEUVER>& compatibilityUnderThreshold)
{

	List<Maneuver> maneuversLeft;

	// if at least one hypothesis is left, return true
	for (int sigma = 0; sigma < N_MANEUVER; sigma++)
	{

		bool foundManeuver = false;

		for (int list = 0; list < compatibilityUnderThreshold[sigma].count(); list++)
		{
			const Tensor5<bool>* comp;
			compatibilityUnderThreshold[sigma].getElem(comp, list);

			for (int i = 0; i < comp->Dim1; i++)
			{
				for (int j = 0; j < comp->Dim2; j++)
				{
					for (int k = 0; k < comp->Dim3; k++)
					{
						for (int l = 0; l < comp->Dim4; l++)
						{
							for (int m = 0; m < comp->Dim5; m++)
							{
								if ((*comp)(i, j, k, l, m) == true)
									foundManeuver = true;

								if (foundManeuver)
									break;
							}

							if (foundManeuver)
								break;
						}

						if (foundManeuver)
							break;
					}

					if (foundManeuver)
						break;
				}

				if (foundManeuver)
					break;
			}

			if (foundManeuver)
				break;

		}
		if (foundManeuver)
			maneuversLeft.insTail((Maneuver)sigma);
	}



	return maneuversLeft;

}
