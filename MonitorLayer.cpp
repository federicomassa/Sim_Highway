/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        MonitorLayer.cpp
 * DESCRIPTION: Implementation of the MonitorLayer class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "MonitorLayer.h"
#include "Predictor.h"
#include "Grid.h"

MonitorLayer::~MonitorLayer()
{
    /* delete the monitors in the list */
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        delete mon;
}

Monitor* MonitorLayer::addMonitor(int t, const State& tQ, const Parms& tP, const Maneuver& tSigma, const List<State>& qList)
{
  Monitor* mon = new Monitor(agentID, t, tQ, tP, tSigma, qList);
  monitorList.insHead(mon);
  return mon;
}

Monitor* MonitorLayer::lookFor(int t)
{
    Iterator<Monitor*> i(monitorList);
    Monitor* mon;
    while(i(mon))
        if(mon->getTargetID() == t)
            return mon;

    return NULL;
}

bool MonitorLayer::removeMonitor(int t)
{
    Monitor* m = lookFor(t);
    if(m == NULL)
        return false;

    monitorList.delInfo(m);
    delete m;

    return true;
}

void MonitorLayer::run(const List<Sensing>& sList, const State& agentQ, const Maneuver& agentManeuver,
                       const Area& obs)
{
    if(CONF.debug)
    {
        LOG.s << "Monitor Layer " << agentID << " START...";
        LOG.s << EndLine(EndLine::INC);
    }
    Iterator<Sensing> i(sList);
    Sensing s;
    List<int> processedID;
    while(i(s))
    {
        /* check if the target id is in the active targets list */
        if(!CONF.allTargetsActive && !CONF.activeTargets.belongs(s.agentID))
            continue;
        
        processedID.insHead(s.agentID);

	// building state list 
	Iterator<Sensing> is(sList);
	Sensing tmpS;
	List<State> qList;
	while(is(tmpS))
	  if(tmpS != s)
	    qList.insHead(tmpS.q);
	// insert monitor agent state in the list
	qList.insHead(agentQ);
	
        Monitor* m = lookFor(s.agentID);
        if(m == NULL)
        {
	  m = addMonitor(s.agentID, s.q, s.p, s.sigma, qList);
	}
	//else
	
	/* detect target maneuver */
	m->detectManeuver(s.q, s.sigma);
	
	/* predict possible maneuvers */
	m->predictManeuvers(qList, obs);

	/* Prediction --- FIXME Testing for now */
	
	if (m->getAgentID() == 0 && m->getTargetID() == 1)
	  {
	    Predictor predictor(agentID, agentQ, agentManeuver, sList, m->getTargetID(), 1);
	    Vector<List<Tensor5<Sensing> >, N_MANEUVER> monitorPrediction;
	    Vector<List<Tensor5<Vector<double, 4> > >, N_MANEUVER> errors;

	    if (!isTargetLocked)
	      std::cout << "Real Monitor maneuver: " << s.sigma << std::endl;
	    
	    predictor.run();
	    
	    if (isTargetLocked)
	      {
	
		m->getMonitorPrediction(monitorPrediction);
		m->getErrors(errors);
		
		Maneuver detectedManeuver;
		State monitorQ;
		m->getTargetQ(monitorQ);
		
		Sensing monitorS(m->getTargetID(), monitorQ, monitorQ.v, FAST);
		if (predictor.detectManeuver(monitorS, monitorPrediction, errors, detectedManeuver))
		  std::cout << "Detected Maneuver" << detectedManeuver << std::endl;
		else
		  std::cout << "Not detected" << std::endl;
		
		std::cout << "After detect" << std::endl;
		
		/*
		  Vector<double, 4> err;
		  for (int i = 0; i < monitorPrediction[(int)PLATOON].count(); i++)
		  {
		  const Tensor5<Sensing>* elem;
		  monitorPrediction[(int)PLATOON].getElem(elem, i);
		  for (int j = 0; j < elem->Dim1; j++)
		  for (int k = 0; k < elem->Dim2; k++)
		  for (int l = 0; l < elem->Dim3; l++)
		  for (int m = 0; m < elem->Dim4; m++)
		  for (int n = 0; n < elem->Dim5; n++)
		  {
		  predictor.getError(err, i, j, k, l, m, n, PLATOON);
		  std::cout << "Errors: " << err[0] << '\t' << err[1] << '\t' << err[2] << '\t' << err[3] << std::endl;
		  }
		  
		  }
		  
		*/
		
		Grid g;
		for (int i = 0; i < N_MANEUVER; i++)
		  for (int v1 = 0; v1 < 4; v1++)
		    for (int v2 = v1 + 1; v2 < 4; v2++)
		      {
			g.drawStateSpace(monitorPrediction[i], v1, v2);
			// C**time** A**agent** T**target** M**maneuver** V**pair of variables**
			//g.save("Pred", "A" + toString(m->getAgentID(),2) + "T" + toString(m->getTargetID(),2) + "M" + toString(i) + "V" + toString(v1) + toString(v2));
		      }
	      }
	    
	    predictor.getMonitor(monitorPrediction);
	    predictor.getErrors(errors);
	    m->setMonitorPrediction(monitorPrediction);
	    m->setErrors(errors);
	
	
	  }
	
	
    }


    isTargetLocked = true;
    
    if(CONF.debug)
      LOG.s << "Deleting unused monitors: ";
    /* deleting unused monitors */
    Iterator<Monitor*> im(monitorList);
    Monitor* m;
    while(im(m))
        if(!processedID.belongs(m->getTargetID()))
        {
            if(CONF.debug)
                LOG.s << m->getAgentID() << ' ';
            monitorList.delInfo(m);
            delete m;
        }
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC);
        LOG.s << "Monitor Layer " << agentID << " STOP.";
    }
}

void MonitorLayer::getHypothesis(List<Hypothesis>& hList) const
{
    /* error handling */
    if(hList.count() > 0)
        error("MonitorLayer", "Hypothesis list MUST be empty");
    
    Iterator<Monitor*> i(monitorList);
    Monitor* m;
    while(i(m))
    {
        Hypothesis hyp;
        if(m->getFirstHypothesis(hyp))
            hList.insHead(hyp);
    }
}

void MonitorLayer::buildNeighborhoodList(List<Neighborhood>& nList) const
{
    /* error handling */
    if(nList.count() > 0)
        error("MonitorLayer", "Neighborhood list MUST be empty");
    
    Iterator<Monitor*> i(monitorList);
    Monitor* m;
    while(i(m))
    {
        Neighborhood n;
        if(m->buildNeighborhood(n))
	  {
            nList.insHead(n);
	  }
    }


    
    
}
