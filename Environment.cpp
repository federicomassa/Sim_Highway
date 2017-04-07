#include "Environment.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

typedef Vector<Vector<double, 2>, 2> Matrix_2x2;

Environment::Environment(int n, double r, double p) : repChannel(r, p), nV(n)
{
  v = new Vehicle[nV];  
}

void Environment::initVehicles(const List<pair<State, Parms> >& qpL)
{
    /* error handling */
    if(qpL.count() != nV)
        error("Environment", "invalid state-parms list lenght");
    
    Iterator<pair<State, Parms> > iQP(qpL);
    pair<State, Parms> tmpQP;

    for(int i = 0; i < nV && iQP(tmpQP); i++)
      v[i].init(tmpQP);

    for(int i = 0; i < nV; i++)
      {
	v[i].setID(i);
	v[i].initRM(&repChannel);
      }
    
}

/**
 * this function returns the i-th agent's state
 */
State Environment::getQ(int index) const
{
    /* error handling */
    if (index >= nV)
        error("Environment", "vehicle index exceeded Environment bounds");

    return v[index].getQ();
}

Maneuver Environment::getManeuver(int index) const
{
    /* error handling */
    if (index >= nV)
        error("Environment", "vehicle index exceeded Environment bounds");
    
    return v[index].getManeuver();
}

/**
 * this function returns i-th agent's observable area
 */
void Environment::observableArea(int index, Area& obs, Area* hiddenArea, const double& visible_distance) const
{
    /* error handling */
    if (index >= nV || index < 0)
        error("Environment", "vehicle index exceeded Environment bounds");

    const State subj = getQ(index);

    if(CONF.debug)
    {
        LOG.s << "Computing observable area for agentID ";
        LOG.s.fill('0');
        LOG.s.width(2);
        LOG.s << index;
        LOG.s << " BEGIN:" << EndLine(EndLine::INC);
        LOG.s << "Subject state: " << subj << EndLine();
    }
    const double MINIMUM_X = subj.x - visible_distance;
    const double MAXIMUM_X = subj.x + visible_distance;
    
    /* matrix needed for rectangles costructor */
    Matrix_2x2 bounds;

    Area hidden;

    for (int k = 0; k < nV; k++)
    {
        if (k == index)
            continue;
        
        const State s = getQ(k);
        
        if(CONF.debug)
        {
            LOG.s << "Agent ";
            LOG.s.fill('0');
            LOG.s.width(2);
            LOG.s << k << " state " << s << EndLine(EndLine::INC);
        }
        
        if (s.x > MAXIMUM_X || s.x < MINIMUM_X)
        {
            /* s is too far */
            if(CONF.debug)
                LOG.s << "Too far" << EndLine(EndLine::DEC);
            continue;
        }

	/* alpha[] is the angle formed between the subject vehicle and
	   each one of the four vertices of the observed vehicle's image.

	   intersection[] is ?
	
	*/
	
        Vector<double, 4> x, y, alpha, intersection;
        double cosTheta = 1;
        double sinTheta = 0;
        if (s.theta != 0)
        {
	  cosTheta = cos(s.theta);
	  sinTheta = sin(s.theta);
        }
        const double deltaXX = 0.5 * v[k].getPixelWidth() / SCALE * cosTheta;
        const double deltaXY = 0.5 * v[k].getPixelHeight() / SCALE * sinTheta;
        const double deltaYX = 0.5 * v[k].getPixelWidth() / SCALE * sinTheta;
        const double deltaYY = 0.5 * v[k].getPixelHeight() / SCALE * cosTheta;

	
        /* x[] and y[] are the vertexes of vehicle's image */
        x[0] = s.x - deltaXX + deltaXY;
        x[1] = s.x + deltaXX + deltaXY;
        x[2] = s.x + deltaXX - deltaXY;
        x[3] = s.x - deltaXX - deltaXY;
        y[0] = s.y - deltaYX - deltaYY;
        y[1] = s.y + deltaYX - deltaYY;
        y[2] = s.y + deltaYX + deltaYY;
        y[3] = s.y - deltaYX + deltaYY;

        if(CONF.debug)
        {
            LOG.s << "Vertexes:";
            for (int i = 0; i < 4; i++)
                LOG.s << " (" << x[i] << ", " << y[i] << ')';
            LOG.s << EndLine();
        }
        
        const int currLane = (int)floor(subj.y);
        const int otherLane = (int)floor(s.y);

        for (int i = 0; i < 4; i++)
            alpha[i] = atan2(y[i] - subj.y, x[i] - subj.x);

	// lmin, lmax are ?
        int lmin, lmax;
        if (otherLane == currLane)
        {
            lmin = MIN_LANE;
            lmax = MAX_LANE;
        }
        else if (otherLane > currLane)
        {
            lmin = otherLane;
            lmax = MAX_LANE;
        }
        else
        {
            lmin = MIN_LANE;
            lmax = otherLane;
        }
        for (int lane = lmin; lane < lmax + 1; lane++)
        {
            if(CONF.debug)
                LOG.s << "Lane " << lane << ": " << EndLine();
            for (int j = 0; j < 4; j++)
            {
	      // dY is the distance between the examined lane center and the subject y position
                const double dY = (double)lane +  0.5 - subj.y;

		// if the other vehicle is perfectly aligned with ours
                if (alpha[j] == PI/2 || alpha[j] == -PI/2)
                    intersection[j] = subj.x;
                else
                {
                    double tA = tan(alpha[j]);
                    if(tA == 0 || sign(alpha[j]) != sign(dY) || dY == 0)
                    {
                        if (s.x > subj.x)
                            intersection[j] = MAXIMUM_X;
                        else
                            intersection[j] = MINIMUM_X;
                    }
                    else
                        intersection[j] = subj.x + dY / tA;
                }
            }
            intersection.sort();
            if(CONF.debug)
            {
                LOG.s << "Intersections:";
                for (int i = 0; i < 4; i++)
                    LOG.s << ' ' << intersection[i];
                LOG.s << EndLine();
            }
            if (lane == otherLane)
            {
                const double delta = deltaXX + fabs(deltaXY);
                if (s.x > subj.x)
                {
                    if (s.x + delta < intersection[3])
                    {
                        bounds[0][0] = s.x + delta;
                        bounds[0][1] = intersection[3];
                        bounds[1][0] = (double)lane;
                        bounds[1][1] = (double)lane + 1;
                        if(CONF.debug)
                        {
                            LOG.s << "Adding " << Rectangle(bounds);
                            LOG.s << EndLine();
                        }
                        hidden.addRect(bounds);
                    }
                }
                else if (s.x < subj.x)
                {
                    if (intersection[0] < s.x - delta)
                    {
                        bounds[0][0] = intersection[0];
                        bounds[0][1] = s.x - delta;
                        bounds[1][0] = (double)lane;
                        bounds[1][1] = (double)lane + 1;
                        if(CONF.debug)
                        {
                            LOG.s << "Adding " << Rectangle(bounds);
                            LOG.s << EndLine();
                        }
                        hidden.addRect(bounds);
                    }
                }
            }
            else
            {
                bounds[0][0] = intersection[0];
                bounds[0][1] = intersection[3];
                bounds[1][0] = (double)lane;
                bounds[1][1] = (double)lane + 1;
                if(CONF.debug)
                    LOG.s << "Adding " << Rectangle(bounds) << EndLine();
                hidden.addRect(bounds);
            }
        }
        if(CONF.debug)
        {
            LOG.s << "Agent ";
            LOG.s.fill('0');
            LOG.s.width(2);
            LOG.s << k << " END." << EndLine(EndLine::DEC);
        }
    }

    if(CONF.debug)
        LOG.s << "Simplify hidden area...";
    hidden.simplify();
    if(CONF.debug)
        LOG.s << " done." << EndLine();

    // if not null, save hidden area
    if (hiddenArea)
      *hiddenArea = hidden;
    
    /* building maximum observable area as a bounding box */
    bounds[0][0] = MINIMUM_X;
    bounds[0][1] = MAXIMUM_X;
    bounds[1][0] = MIN_LANE;
    bounds[1][1] = MAX_LANE + 1;
    Area bBox;
    bBox.addRect(bounds);
    
    if(CONF.debug)
        LOG.s << "Hidden area: " << hidden << EndLine();
    obs = bBox - hidden;
    if(CONF.debug)
    {
        LOG.s << "Observable area: " << obs << EndLine(EndLine::DEC);
        LOG.s << "END." << EndLine();
    }
}

void Environment::run()
{ 
  Area* obsAreas = new Area[nV];
  List<Sensing>* sLists = new List<Sensing>[nV];
  
  /* update states */
  for(int i = 0; i < nV; i++)
    v[i].run();
  
  /* simulate */
  for(int i = 0; i < nV; i++)
    {
      if(CONF.debug)
        {
	  LOG.s << "Evolving vehicle ";
	  LOG.s.width(2);
	  LOG.s.fill('0');
	  LOG.s << i << ':' << EndLine(EndLine::INC);
        }
      
      Area obs;
      observableArea(i,obs);
      obsAreas[i] = obs;

      
      List<Sensing> sList;
      
      if(CONF.debug)
        {
	  LOG.s << "Evaluating vehicles inside active area:";
	  LOG.s << EndLine(EndLine::INC);
        }
      for(int j = 0; j < nV; j++)
        {
	  // vehicles seen by the subject are the ones within observable area
	  if(j != i && v[j].inArea(obs))
            {
	      double x = v[j].getQ().x + SIGMA_X*sqrt(12)*(double)rand()/(double)RAND_MAX - SIGMA_X*sqrt(12)/2;
	      double y = v[j].getQ().y + SIGMA_Y*sqrt(12)*(double)rand()/(double)RAND_MAX - SIGMA_Y*sqrt(12)/2;
	      double theta = v[j].getQ().theta + SIGMA_THETA*sqrt(12)*(double)rand()/(double)RAND_MAX - SIGMA_THETA*sqrt(12)/2;
	      double vel = v[j].getQ().v + SIGMA_V*sqrt(12)*(double)rand()/(double)RAND_MAX - SIGMA_V*sqrt(12)/2;
	      
	      if (vel > MAX_SPEED)
		vel = MAX_SPEED;
	      if (vel < 0)
		vel = 0;
	      
	      State errQ(x,y,theta,vel);
	      
	      Sensing tmpS(v[j].getID(), errQ, v[j].getParms());
	      sList.insHead(tmpS);
            }
        }

      sLists[i] = sList;
      
      if(CONF.debug)
	LOG.s << sList << EndLine(EndLine::DEC);
      
      if(CONF.debug)
	LOG.s << EndLine(EndLine::DEC);
    }

  for (int i = 0; i < nV; i++)
    {
      /* stimulate vehicle */
      v[i].preRun(sLists[i], obsAreas[i]);
    }
  
  for (int i = 0; i < nV; i++)
    {
      /* After each vehicle has evolved, run monitor */
      v[i].evolveMonitor(obsAreas[i]);
    }

  delete[] sLists;
  delete[] obsAreas;

}

void Environment::omniscientNeighborhoodList(List<Neighborhood>& oNL)
{
    ReputationManager oRM; /* omniscient reputation manager */
    for(int i = 0; i < nV; i++)
    {
        Knowledge k = v[i].getKnowledge();
        oRM.singleMerge(k);
    }
    oRM.getNeighborhoodList(oNL);
}

void Environment::consensusStep()
{
  std::cout << "consensus step" << std::endl;
    for(int i = 0; i < nV; i++)
        v[i].sendRM();
    if(CONF.debug)
        LOG.s << "Reputation channel:" << EndLine()
            << repChannel << EndLine();
    for(int i = 0; i < nV; i++)
        v[i].recvRM();
    repChannel.clearAll();

    /* If a countdown was received, set other observers to wait for the desired time to sync */
    for (int i = 0; i < nV; i++)
      {
	v[i].shareWaitingList();
      }
}

void Environment::activateMonitorLayer(int index)
{
    /* error handling */
    if (index >= nV)
        error("Environment", "vehicle index exceded Environment bounds");

    v[index].activateMonitorLayer();
}

void Environment::activateReputationManager(int index)
{
    /* error handling */
    if (index >= nV)
        error("Environment", "vehicle index exceded Environment bounds");

    v[index].activateReputationManager();
}

void Environment::getHypothesis(int index, List<Hypothesis>& hypList) const
{
    v[index].getHypothesis(hypList);
}

void Environment::outputNeighborhoodsMu(int cStep) const
{
    for(int i = 0; i < nV; i++)
    {
        if(!CONF.activeReputationManagers.belongs(i))
            continue;
        
        List<Neighborhood> nList;
        v[i].getNeighborhoodList(nList);
        Iterator<Neighborhood> ni(nList);
        Neighborhood n;
        while(ni(n))
        {
            string fileName = "Mu-T" + toString(n.getTargetID(), 2);
            Output out(fileName, 4);
            out.insert(now);
            out.insert(cStep);
            out.insert(i);
            out.insert(n.measure());
        }
    }
}

void Environment::outputTargetsReputation(int cStep) const
{
    for(int i = 0; i < nV; i++)
    {
        if(!CONF.activeReputationManagers.belongs(i))
            continue;
        
        List<Reputation> repList;
        v[i].getAgentReputationList(repList);
        Iterator<Reputation> ri(repList);
        Reputation r;
        while(ri(r))
        {
            string fileName = "Reputation-T" + toString(r.targetID, 2);
            Output out(fileName, 4);
            out.insert(now);
            out.insert(cStep);
            out.insert(i);
            double l = (double)r.level / 2.0;
            out.insert(l);
        }
    }
}

void Environment::outputNeighborhoodsOverhead(int cStep) const
{
    for(int i = 0; i < nV; i++)
    {
        if(!CONF.activeReputationManagers.belongs(i))
            continue;
        
        List<Neighborhood> nList;
        v[i].getNeighborhoodList(nList);
        if(!nList.isEmpty())
        {
            string fileName = "NeighOverhead";
            Output out(fileName, 4);
            out.insert(now);
            out.insert(cStep);
            out.insert(i);
            out.insert(size(nList));
        }
    }
}

void Environment::outputVehiclesStates() const
{
    for(int i = 0; i < nV; i++)
    {
        State q = getQ(i);
        Maneuver m = getManeuver(i);
        string fileName = "Evolution-A" + toString(i, 2);
        Output out(fileName, 6);
        out.insert(now);
        out.insert(q.x);
        out.insert(q.y);
        out.insert(q.theta);
        out.insert(q.v / MAX_SPEED);
        out.insert((int)m);
    }
}

Vehicle* Environment::getVehicleFromID(const int& id) const
{
  for (int i = 0; i < nV; i++)
    {
      if (v[i].getID() == id)
	return &v[i];
    }

  return NULL;
}
