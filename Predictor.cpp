/*
  Implementation of class Predictor
 */

/*FIXME Add hypothesis with no hidden vehicles */


#include <cmath>
#include <iostream>
#include <iomanip>

#include "Predictor.h"
#include "Environment.h"
#include "DynVector.h"
#include "systemParms.h"
#include "utility.h"

const double Predictor::compatibilityCut = 10;

/* Function to compute the indexes needed for the numerical derivative calculus */
void computeIndexes(const int& i, const int& dim, int& iPlus, int& iMinus)
{
  if (i == 0)
    {
      iPlus = 1;
      iMinus = 0;
    }
  else if(i == (dim - 1))
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

Predictor::Predictor(const int& aID, const State& aState, const Maneuver& aManeuver, const List<Sensing>& sL, const int& monitorIndex, const int& nTSteps) : agentID(aID), iAgentState(aState), agentManeuver(aManeuver), sList(sL), monitorID(monitorIndex), nTimeSteps(nTSteps)
{
  if (nTSteps <= 0)
    error("Predictor", "Invalid number of time steps, should be > 0");

  // Initialize hidden state vector with fixed spacing
  deltaX = VISIBLE_DISTANCE/10;
  deltaY = LANE_HEIGHT/4;
  deltaTheta = 2*MAX_THETA/3;
  deltaV = 1.0/10.0;
  deltaDesiredV = 1.0/10.0;
  
  //set starting time
  currentStep = 0;
  
  /*  // Fill agentState vector with the initial condition
  Tensor5<State> agentTensor(1,1,1,1);
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
	
	Tensor5<Sensing> cTensor(1,1,1,1);
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


  /* Add an empty rectangle to the TOP OF THE LIST (important for error computation), that represents the hypothesis
   with no hidden vehicles -> tensor(1,1,1,1,1) */
  Rectangle dummyRect;
  rList.insHead(dummyRect);

  // loop on the possible monitor maneuvers
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
    {

      Iterator<Rectangle> iRect(rList);
      Rectangle tmpRect;
      
      while(iRect(tmpRect))
	{
	  bool noHiddenVehicle = false;
	  
	  if (tmpRect.isDummy)
	    noHiddenVehicle = true;
	  
	  Vector<Vector<double,2>, 2> b;
	  tmpRect.getBounds(b);
	  
	  // rectangle bounds
	  double xmin = b[0][0];
	  double xmax = b[0][1];
	  double ymin = b[1][0];
	  double ymax = b[1][1];
	  
	  if ((xmax <= xmin || ymax <= ymin) && !noHiddenVehicle)
	    continue;
	  
	  // n bins 
	  int nX, nY, nTheta, nV, nDesiredV;
	  
	  // if the ratio is not exact, we also take the smaller remainder : x_____x_____x_____x___x
	  if (!noHiddenVehicle)
	    {
	      nX = ceil((xmax - xmin)/deltaX) + 1;
	      nY = ceil((ymax - ymin)/deltaY) + 1;
	      nTheta = ceil(2*MAX_THETA/deltaTheta) + 1;
	      nV = ceil(1.0/deltaV) + 1;
	      nDesiredV = ceil(1.0/deltaDesiredV) + 1;
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
	

	  // Hypothesis on a grid
	  for (int xi = 0; xi < nX; xi++)
	    for (int yi = 0; yi < nY; yi++)
	      for (int thetai = 0; thetai < nTheta; thetai++)
		for (int vi = 0; vi < nV; vi++)
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
		      
		      
		      if (xi != nX -1)
			x = xmin + xi*deltaX;
		      else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
			x = xmax;
		      
		      if (yi != nY -1)
			y = ymin + yi*deltaY;
		      else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
			y = ymax;
		      
		      if (thetai != nTheta -1)
			theta = -MAX_THETA + thetai*deltaTheta;
		      else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
			theta = MAX_THETA;
		      
		      if (vi != nV -1)
			v = 0 + vi*deltaV;
		      else //this case is to account for possible remainder in the ratio. If exact, doesn't change anything
			v = 1.0;
		      
		      if (desiredVi != nDesiredV -1)
			desiredV = 0 + desiredVi*deltaDesiredV;
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
			  if(simulEnv->getVehicles()[n].getID() == monitorID)
			    {
			      monitorV = &(simulEnv->getVehicles()[n]);
			      monitorIndex = n;
			      break;
			    }
			}
		      
		      if (monitorV == NULL)
			error("Predictor", "Monitor not found in environment");
		      
		      State monitorQ = monitorV->getQ();
		      
		      /* Set monitor maneuver */
		      monitorQ.initManeuver = maneuverToStr((Maneuver)sigma);
		      monitorPLayer.init(monitorQ, monitorQ.v);
		      agentPLayer.init(iAgentState, iAgentState.v);
		      
		      /* Predict each vehicle's behaviour within nTSteps time steps. 
			 Hypothesis: all vehicles except observer and monitored move with const speed */
		      for (int steps = 0; steps < nTimeSteps; steps++)
			{
			  //create monitor qList
			  List<State> monitorQList;
			  List<State> agentQList;
			  
			  /* Evaluate monitored vehicle's observable area */
			  Area monitorObs;
			  simulEnv->observableArea(monitorIndex, monitorObs);
			  
			  for (int n = 0; n < simulEnv->getNVehicles(); n++)
			    {
			      if (simulEnv->getVehicles()[n].getID() != monitorID && simulEnv->getVehicles()[n].inArea(monitorObs))
				monitorQList.insHead(simulEnv->getVehicles()[n].getQ());
			      
			      if (simulEnv->getVehicles()[n].getID() != agentID && simulEnv->getVehicles()[n].inArea(obs))
				agentQList.insHead(simulEnv->getVehicles()[n].getQ());
			      
			    }
			  
			  //monitorPLayer.updateQ();
			  monitorPLayer.computeNextQ((Maneuver)sigma, monitorQList);
			  agentPLayer.computeNextQ((Maneuver)agentManeuver, agentQList);
			  
			  /* update pLayer for next iteration */
			  monitorPLayer.updateQ();
			  agentPLayer.updateQ();
			  
			  /* evolve remaining vehicles */
			  for (int n = 0; n < simulEnv->getNVehicles(); n++)
			    {
			      if (simulEnv->getVehicles()[n].getID() == agentID ||
				  simulEnv->getVehicles()[n].getID() == monitorID)
				continue;
			      
			      Vehicle& veh = simulEnv->getVehicles()[n];
			      State oldQ = simulEnv->getVehicles()[n].getQ();
			      /* uniform speed motion */
			      State newQ(oldQ.x + oldQ.v*cos(oldQ.theta),
					 oldQ.y + oldQ.v*sin(oldQ.theta),
					 oldQ.theta,
					 oldQ.v);
			      
			      veh.setQ(newQ);
			      
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
		      
		    }
	    
	  if (!noHiddenVehicle)
	    hiddenState[sigma].insTail(tmpHidden);
	  
	  monitorState[sigma].insTail(tmpMonitor);
	  
	}


      /* Now compute errors */
      List<Tensor5<Vector<double, 4> > > errList;
    
      for (int list = 0; list < monitorState[sigma].count(); list++)
	{
	  bool noHiddenVehicle = false;
	  
	  const Tensor5<Sensing>* monitor;
	  monitorState[sigma].getElem(monitor, list);
	  
	  Tensor5<Vector<double,4> > errTens(monitor->Dim1, monitor->Dim2, monitor->Dim3, monitor->Dim4, monitor->Dim5);

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
			getError(tmpErr, monitor, i, j, k, l, m);
		      else
			{
			  tmpErr[0] = 0;
			  tmpErr[1] = 0;
			  tmpErr[2] = 0;
			  tmpErr[3] = 0;
			}
		      
		      errTens(i,j,k,l,m) = tmpErr;

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

      errors[sigma] = errList;
    }


  
  // increment time
  currentStep++;
  
  
}

void Predictor::getError(Vector<double, 4>& err, const int& listIndex,
			 const int& i, const int& j, const int& k, const int& l, const int& m,
			 const Maneuver& monitorSigma)
{
  const Tensor5<Sensing>* monitor;
  
  monitorState[(int)monitorSigma].getElem(monitor, listIndex);

  getError(err, monitor, i, j, k, l, m);
}

void Predictor::getError(Vector<double, 4>& err, const Tensor5<Sensing>* monitor,
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
  errX += pow(((*monitor)(i+iPlus, j, k, l, m).q.x - (*monitor)(i+iMinus,j,k,l,m).q.x)/double(iPlus - iMinus), 2);
  errX += pow(((*monitor)(i, j+jPlus, k, l, m).q.x - (*monitor)(i,j+jMinus,k,l,m).q.x)/double(jPlus - jMinus), 2);
  errX += pow(((*monitor)(i, j, k+kPlus, l, m).q.x - (*monitor)(i,j,k+kMinus,l,m).q.x)/double(kPlus - kMinus), 2);
  errX += pow(((*monitor)(i, j, k, l+lPlus, m).q.x - (*monitor)(i,j,k,l+lMinus,m).q.x)/double(lPlus - lMinus), 2);
  errX += pow(((*monitor)(i, j, k, l, m+mPlus).q.x - (*monitor)(i,j,k,l,m+mMinus).q.x)/double(mPlus - mMinus), 2);

  errY += pow(((*monitor)(i+iPlus, j, k, l, m).q.y - (*monitor)(i+iMinus,j,k,l,m).q.y)/double(iPlus - iMinus), 2);
  errY += pow(((*monitor)(i, j+jPlus, k, l, m).q.y - (*monitor)(i,j+jMinus,k,l,m).q.y)/double(jPlus - jMinus), 2);
  errY += pow(((*monitor)(i, j, k+kPlus, l, m).q.y - (*monitor)(i,j,k+kMinus,l,m).q.y)/double(kPlus - kMinus), 2);
  errY += pow(((*monitor)(i, j, k, l+lPlus, m).q.y - (*monitor)(i,j,k,l+lMinus,m).q.y)/double(lPlus - lMinus), 2);
  errY += pow(((*monitor)(i, j, k, l, m+mPlus).q.y - (*monitor)(i,j,k,l,m+mMinus).q.y)/double(mPlus - mMinus), 2);
  
  errTheta += pow(((*monitor)(i+iPlus, j, k, l, m).q.theta - (*monitor)(i+iMinus,j,k,l,m).q.theta)/double(iPlus - iMinus), 2);
  errTheta += pow(((*monitor)(i, j+jPlus, k, l, m).q.theta - (*monitor)(i,j+jMinus,k,l,m).q.theta)/double(jPlus - jMinus), 2);
  errTheta += pow(((*monitor)(i, j, k+kPlus, l, m).q.theta - (*monitor)(i,j,k+kMinus,l,m).q.theta)/double(kPlus - kMinus), 2);
  errTheta += pow(((*monitor)(i, j, k, l+lPlus, m).q.theta - (*monitor)(i,j,k,l+lMinus,m).q.theta)/double(lPlus - lMinus), 2);
  errTheta += pow(((*monitor)(i, j, k, l, m+mPlus).q.theta - (*monitor)(i,j,k,l,m+mMinus).q.theta)/double(mPlus - mMinus), 2);

  errV += pow(((*monitor)(i+iPlus, j, k, l, m).q.v - (*monitor)(i+iMinus,j,k,l,m).q.v)/double(iPlus - iMinus), 2);
  errV += pow(((*monitor)(i, j+jPlus, k, l, m).q.v - (*monitor)(i,j+jMinus,k,l,m).q.v)/double(jPlus - jMinus), 2);
  errV += pow(((*monitor)(i, j, k+kPlus, l, m).q.v - (*monitor)(i,j,k+kMinus,l,m).q.v)/double(kPlus - kMinus), 2);
  errV += pow(((*monitor)(i, j, k, l+lPlus, m).q.v - (*monitor)(i,j,k,l+lMinus,m).q.v)/double(lPlus - lMinus), 2);
  errV += pow(((*monitor)(i, j, k, l, m+mPlus).q.v - (*monitor)(i,j,k,l,m+mMinus).q.v)/double(mPlus - mMinus), 2);

  err[0] = sqrt(errX/12.0);
  err[1] = sqrt(errY/12.0);
  err[2] = sqrt(errTheta/12.0);
  err[3] = sqrt(errV/12.0); 
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
  Vector<double, 4> err = (*errTens)(i,j,k,l,m);
  
  double compatibility = 0;

  /* Definition of compatibility of a point with the measurement: sqrt of sum of squares of (x_i - x_meas)/errX + ... */

  /* Special case: if error is 0 and the distance from the measurement is != 0 => not compatible (large number) */
  if (err[0] < 1E-10)
    {

      
      if (fabs((*monitor)(i,j,k,l,m).q.x - measure.q.x) > 1E-10)
	return 1000000;
      /*      else
	      std::cout << "DiffX: " << fabs((*monitor)(i,j,k,l,m).q.x - measure.q.x) << std::endl;*/
    }
  else
    compatibility += pow(((*monitor)(i,j,k,l,m).q.x - measure.q.x)/err[0], 2);

  if (err[1] < 1E-10)
    {

      if (fabs((*monitor)(i,j,k,l,m).q.y - measure.q.y) > 1E-10)
	return 1000000;
      /*      else
	      std::cout << "DiffY: " << fabs((*monitor)(i,j,k,l,m).q.y - measure.q.y) << std::endl;*/
    }
  else
    compatibility += pow(((*monitor)(i,j,k,l,m).q.y - measure.q.y)/err[1], 2);

  
  if (err[2] < 1E-10)
    {

      if (fabs((*monitor)(i,j,k,l,m).q.theta - measure.q.theta) > 1E-10)
	return 1000000;
      /*      else
	      std::cout << "DiffT: " << fabs((*monitor)(i,j,k,l,m).q.theta - measure.q.theta) << std::endl;*/
    }
  else
    compatibility += pow(((*monitor)(i,j,k,l,m).q.theta - measure.q.theta)/err[2], 2);
  
  if (err[3] < 1E-10)
    {
      if (fabs((*monitor)(i,j,k,l,m).q.v - measure.q.v) > 1E-10)
	return 1000000;
      /*      else
	      std::cout << "DiffV: " << fabs((*monitor)(i,j,k,l,m).q.v - measure.q.v) << std::endl;*/
    }
  else
    compatibility += pow(((*monitor)(i,j,k,l,m).q.v - measure.q.v)/err[3], 2);
  
  
  compatibility = sqrt(compatibility);
  
  return compatibility;

  
}


void Predictor::compareWithMeasure(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
				   const Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER>& errV,
				   Vector<List<Tensor5<double> >, N_MANEUVER>& compatibility)
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

      compatibility[sigma] = compList;
      
    }
  
}

void Predictor::chooseBestPrediction(const Vector<List<Tensor5<double> >, N_MANEUVER>& compatibility, Vector<double, N_MANEUVER>& bestCompatibility,
				     Vector<int, N_MANEUVER>& bestList, Vector<int, N_MANEUVER>& iV, Vector<int, N_MANEUVER>& jV,
				     Vector<int, N_MANEUVER>& kV, Vector<int, N_MANEUVER>& lV, Vector<int, N_MANEUVER>& mV)
{

  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
    {
      bestCompatibility[sigma] = 1E9;
      
      for (int list = 0; list < compatibility[sigma].count(); list++)
	{
	  const Tensor5<double>* comp;
	  compatibility[sigma].getElem(comp, list);

	  for (int i = 0; i < comp->Dim1; i++)
	    for (int j = 0; j < comp->Dim2; j++)
	      for (int k = 0; k < comp->Dim3; k++)
		for (int l = 0; l < comp->Dim4; l++)
		  for (int m = 0; m < comp->Dim5; m++)
		    {
		      if ((*comp)(i,j,k,l,m) < bestCompatibility[sigma])
			{
			  bestCompatibility[sigma] = (*comp)(i,j,k,l,m);
			  bestList[sigma] = list;
			  iV[sigma] = i;
			  jV[sigma]= j;
			  kV[sigma] = k;
			  lV[sigma] = l;
			  mV[sigma] = m;
			}

		      if (i == 1 && j == 0 && k == 0 && l == 5 && m == 8)
			std::cout << "Speravo: " << (*comp)(i,j,k,l,m) << std::endl;
		    }
	  
	}
      
    }
  
}


bool Predictor::detectManeuver(const Sensing& measure, const Vector<List<Tensor5<Sensing> >, N_MANEUVER>& monitorPrediction,
			       const Vector<List<Tensor5<Vector<double,4> > >, N_MANEUVER>& errV,
			       Maneuver& detectedManeuver)
{

  Vector<List<Tensor5<double> >, N_MANEUVER> compatibility;
  compareWithMeasure(measure, monitorPrediction, errV, compatibility);

  Vector<double, N_MANEUVER> bestCompatibility;
  Vector<int, N_MANEUVER> bestList, iV, jV, kV, lV, mV;

  DynVector<Maneuver> detectedManeuvers;

  chooseBestPrediction(compatibility, bestCompatibility,
		       bestList, iV, jV, kV, lV, mV);

  
  double bestComp = 2E10;
  
  for (int sigma = 0; sigma < N_MANEUVER; sigma++)
    {
      if (bestCompatibility[sigma] < bestComp)
	{
	  detectedManeuver = (Maneuver)sigma;
	  bestComp = bestCompatibility[sigma];
	}

      if (bestCompatibility[sigma] < compatibilityCut)
	detectedManeuvers.insert((Maneuver)sigma);

      /* FIXME Debug*/
      std::cout << "Best comp: " << bestCompatibility[sigma] << '\t' << iV[sigma] << '\t' << jV[sigma] << '\t' << kV[sigma] << '\t' << lV[sigma] << '\t' << mV[sigma] << std::endl;
      const Tensor5<Vector<double, 4> >* tens;
      errV[sigma].getElem(tens, bestList[sigma]);
      std::cout << "ErrX: " << (*tens)(iV[sigma], jV[sigma], kV[sigma], lV[sigma], mV[sigma])[0] << '\t';
      std::cout << "ErrY: " << (*tens)(iV[sigma], jV[sigma], kV[sigma], lV[sigma], mV[sigma])[1] << '\t';
      std::cout << "ErrT: " << (*tens)(iV[sigma], jV[sigma], kV[sigma], lV[sigma], mV[sigma])[2] << '\t';
      std::cout << "ErrV: " << (*tens)(iV[sigma], jV[sigma], kV[sigma], lV[sigma], mV[sigma])[3] << std::endl;
      
    }

  if (detectedManeuvers.length == 1)
    {
      detectedManeuver = detectedManeuvers[0];
      return true;
    }
  else
    return false;
  
}
