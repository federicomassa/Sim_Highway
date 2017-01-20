/*
  Implementation of class Predictor
 */
#include <cmath>

#include "Predictor.h"
#include "Environment.h"
#include "DynVector.h"
#include "systemParms.h"

Predictor::Predictor(const int& aID, const State& aState, const Maneuver& aManeuver, const List<Sensing>& sL, const int& monitorIndex, const int& nTSteps) : agentID(aID), iAgentState(aState), agentManeuver(aManeuver), sList(sL), monitorID(monitorIndex), nTimeSteps(nTSteps)
{
  if (nTSteps <= 0)
    error("Predictor", "Invalid number of time steps, should be > 0");

  // Initialize hidden state vector with fixed spacing
  deltaX = VISIBLE_DISTANCE/8;
  deltaY = LANE_HEIGHT/3;
  deltaTheta = 2*MAX_THETA/3;
  deltaV = MAX_SPEED/5;
  
  
  //set starting time
  currentStep = 0;
  
  /*  // Fill agentState vector with the initial condition
  Tensor4<State> agentTensor(1,1,1,1);
  agentTensor[0][0][0][0] = aState;
  
  for (int i = 0; i < N_MANEUVER; i++)
    {
      agentState[i].init(1,1,1,1);
      agentState[i] = aTensor;
    }
  
  List<Sensing> sListCopy = sList;
  sListCopy.sort();
  
  // fill currentState vector with the initial condition
  for (int i = 0; i < N_MANEUVER; i++)
    for (int j = 0; j < sList.count(); j++)
      {
	Sensing vS;
	sListCopy.extrHead(vS);
	
	Tensor4<Sensing> cTensor(1,1,1,1);
	cTensor(0,0,0,0) = vS;
	
	currentState[i][j] = cTensor;
      }
  */
  
}


Predictor::~Predictor() {}

void Predictor::run()
{
  if (currentStep == nTimeSteps)
    error("Predictor", "Predictor run after maximum time");

  //Temporary environment to compute agent hidden area, +1 to account for the agent
  Environment tmpEnv(sList.count() + 1, CONF.cRadius, CONF.cProb);

  //Simulated environment with all vehicles, including the one hidden
  Environment simulEnv(sList.count() + 2, CONF.cRadius, CONF.cProb);
  
  //list containing all the visible vehicles
  Sensing agentS(agentID, iAgentState, iAgentState.v, agentManeuver);
  List<Sensing> tmpSList = sList; 
  // now agent is the first
  tmpSList.insHead(agentS);

  List<State> tmpQList;
  Iterator<Sensing> iSensing(tmpSList);
  Sensing tmpS;
  
  List<Parms> tmpPList;

  //need q and p list to initialize vehicles
  while(iSensing(tmpS))
    {
      tmpQList.insTail(tmpS.q);
      tmpPList.insTail(tmpS.q.v);
    }

  tmpEnv.initVehicles(tmpQList, tmpPList);
  Area obs;
  Area* hidden = new Area;
  // NB agent is the first, so index 0
  tmpEnv.observableArea(0, obs, hidden);

  // Get list of rectangles forming the area to
  // compute x,y segmentation

  List<Rectangle> rList;
  hidden->getRectList(rList);

  Iterator<Rectangle> iRect(rList);
  Rectangle tmpRect;

  // loop on the possible monitor maneuvers
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
    {
      
      while(iRect(tmpRect))
	{
	  Vector<Vector<double,2>, 2> b;
	  tmpRect.getBounds(b);
	  
	  // rectangle bounds
	  double xmin = b[0][0];
	  double xmax = b[0][1];
	  double ymin = b[1][0];
	  double ymax = b[1][1];
	  
	  if (xmax <= xmin || ymax <= ymin)
	    continue;
	  
	  // n bins 
	  int nX, nY, nTheta, nV;
	  
	  // if the ratio is not exact, we also take the center of the remaining bin: |__x__|__x__|__x__|_x_|
	  nX = ceil((xmax - xmin)/deltaX);
	  nY = ceil((ymax - ymin)/deltaY);
	  nTheta = ceil(2*MAX_THETA/deltaTheta);
	  nV = ceil(MAX_SPEED/deltaV);
	  
	  Tensor4<Sensing> tmpHidden(nX, nY, nTheta, nV);
	  Tensor4<Sensing> tmpMonitor(nX, nY, nTheta, nV);
	
	  
	  // Hypothesis on a grid
	  for (int xi = 0; xi < nX; xi++)
	    for (int yi = 0; yi < nY; yi++)
	      for (int thetai = 0; thetai < nTheta; thetai++)
		for (int vi = 0; vi < nV; vi++)
		  {
		    //check for very small rect case
		    // compute state value taking the center of each bin
		    double x, y, theta, v;
		    
		    
		    if (xi != nX -1)
		      x = (xmin + deltaX/2) + xi*deltaX;
		    else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
		      x = xmin + (nX - 1)*deltaX + (xmax - (xmin + (nX - 1)*deltaX))/2;
		    
		    if (yi != nY -1)
		      y = (ymin + deltaY/2) + yi*deltaY;
		    else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
		      y = ymin + (nY - 1)*deltaY + (ymax - (ymin + (nY - 1)*deltaY))/2;
		    
		    if (thetai != nTheta -1)
		      theta = (-MAX_THETA + deltaTheta/2) + thetai*deltaTheta;
		    else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
		      theta = -MAX_THETA + (nTheta - 1)*deltaTheta + (MAX_THETA - (-MAX_THETA + (nTheta - 1)*deltaTheta))/2;
		    
		    if (vi != nV -1)
		      v = (0 + deltaV/2) + vi*deltaV;
		    else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
		      v = 0 + (nV - 1)*deltaV + (MAX_SPEED - (0 + (nV - 1)*deltaV))/2;
		    
		    //hidden state, with 1 desidered velocity and FAST initial maneuver,
		    // these last two are irrelevant for monitor prediction if only one time step
		    // is considered. FIXME if not
		    State hiddenQ(x, y, theta, v, 1, "FAST");
		    // Simulated sensor response, hidden vehicle as special ID = 100
		    Sensing hiddenS(100, hiddenQ, hiddenQ.v, FAST);
		    Parms hiddenP = hiddenQ.v;
		    // fill tensor
		    tmpHidden(xi, yi, thetai, vi) = hiddenS;
		    
		    //Simulate the environment 
		    tmpSList.insTail(hiddenS);
		    tmpQList.insTail(hiddenQ);
		    tmpPList.insTail(hiddenP);
		    
		    simulEnv.initVehicles(tmpQList, tmpPList);
		    iSensing.initialize(tmpSList);
		    
		    for (int n = 0; n < simulEnv.getNVehicles(); n++)
		      {
			if (iSensing(tmpS))
			  simulEnv.getVehicles()[n].setID(tmpS.agentID);
			else
			  error("Predictor", "Something wrong in sensing list");
		      }
		    
		    //simulEnv.run();
		    
		    Vehicle* monitorV = NULL;
		    for (int n = 0; n < simulEnv.getNVehicles(); n++)
		      {
			if(simulEnv.getVehicles()[n].getID() == monitorID)
			  {
			    monitorV = &simulEnv.getVehicles()[n];
			    break;
			  }
		      }
		    
		    if (monitorV == NULL)
		      error("Predictor", "Monitor not found in environment");
		    
		    State monitorQ = monitorV->getQ();
		    
		    monitorQ.initManeuver = maneuverToStr((Maneuver)sigma);
		    monitorPLayer.init(monitorQ, monitorQ.v);
		    
		    //create monitor qList
		    List<State> monitorQList;
		    for (int n = 0; n < simulEnv.getNVehicles(); n++)
		      {
			if (simulEnv.getVehicles()[n].getID() == monitorID)
			  continue;
			
			monitorQList.insHead(simulEnv.getVehicles()[n].getQ());
		      }
		    
		    monitorPLayer.computeNextQ((Maneuver)sigma, monitorQList);
		    State monitorNextQ = monitorPLayer.getNextQ();

		    Sensing monitorS(monitorID, monitorNextQ, monitorNextQ.v, (Maneuver)sigma);
		    
		    tmpMonitor(xi, yi, thetai, vi) = monitorS;
		    
		    //remove for next iteration
		    tmpSList.extrTail(hiddenS);
		    tmpQList.extrTail(hiddenQ);
		    tmpPList.extrTail(hiddenP);
		    
		  }
	  
	  hiddenState[sigma].insTail(tmpHidden);
	  monitorState[sigma].insTail(tmpMonitor);
	  
	}
    }
  // increment time
  currentStep++;
  
}

   
