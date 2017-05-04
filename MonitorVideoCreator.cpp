#include "MonitorVideoCreator.h"
#include "utility.h"
#include "Image.h"
#include "Environment.h"
#include "LeftAction.h"

#include <gd.h>
#include <cstdio>
#include <string>

MonitorVideoCreator::MonitorVideoCreator(const int& obsID, const int& monID) : observerID(obsID), monitorID(monID)
{}

/* This method creates a frame of the video */
void MonitorVideoCreator::run()
{
  
  /* error handling */
  if (!isInitialized())
    error("MonitorVideoCreator::run", "Video is not initialized");

  if (env == NULL)
    error("MonitorVideoCreator::run", "Environment is not initialized");

  if (globalEnv == NULL)
    error("MonitorVideoCreator::run", "Global Environment is not initialized");
    
  Environment noErrEnv = *env;
  
  Vehicle* monitorV = env->getVehicleFromID(monitorID);
  if (monitorV == NULL)
    return;
  
  Image img(monitorV->getQ().x);


  const int& gray = gdImageColorAllocate(img.getFrame(), 150, 150, 150);
  const int& red = gdImageColorAllocate(img.getFrame(), 255, 0, 0);
  const int& green = gdImageColorAllocate(img.getFrame(), 0, 255, 0);
  
  Area obs;
  Area* hiddenObs = new Area;

  int observerIndex = -1;
  
  for (int i = 0; i < env->getNVehicles(); i++)
    {
      if (env->getVehicles()[i].getID() == observerID)
	{
	  observerIndex = i;
	  break;
	}
    }
  
  env->observableArea(observerIndex, obs, hiddenObs);
  img.drawArea(*hiddenObs, gray);

  const RuleMonitor* rMon;
  if (globalEnv->getVehicleFromID(observerID)->getMonitorLayer()->getMonitor(monitorID))
    rMon = globalEnv->getVehicleFromID(observerID)->getMonitorLayer()->getMonitor(monitorID)->getRuleMonitor();
  else
    return;
  
  List<std::pair<Action*, List<Rule> > > processedActions = rMon->getProcessedActions();

  Iterator<std::pair<Action*, List<Rule> > > aIt(processedActions);
  std::pair<Action*, List<Rule> > a;

  /* for each processed action */
  while (aIt(a))
    {
      /* only for left action */
      if (dynamic_cast<LeftAction*>(a.first))
	{
	  if (now > a.first->endTime && a.first->endTime != -1)
	    {
	      img.drawString("Left action ended", red);
	      break;
	    }

	  img.drawString("Left action triggered", green);
	  
	  Iterator<Rule> rIt(a.second);
	  Rule r;

	  /* for each rule in left */
	  while (rIt(r))
	    {
	      if (r.getCategory() == "Safety")
		continue;
	      
	      List<Event> eList = r.getEventList();
	      Iterator<Event> eIt(eList);
	      Event e;

	      /* for each event in this rule */
	      while (eIt(e))
		{
		  List<SubEvent*> seList = e.getSubEventList();
		  Iterator<SubEvent*> sIt(seList);
		  SubEvent* s;

		  /* for each subevent in this event */
		  while (sIt(s))
		    {
		      if (s->hasArea())
			{
			  Area sArea;
			  Sensing tmpS(monitorV->getID(), monitorV->getQ(),
				       monitorV->getParms());
			  
			  s->evaluateArea(tmpS, sArea);
			  sArea -= obs;
			  
			  if (s->getMode() == OR)
			    img.drawArea(sArea, red);
			  if (s->getMode() == NOR)
			    img.drawArea(sArea, green);
			}
		    }
		  
		}
	    }
	}
    }

  const Vehicle* localVehicles = env->getVehicles();
  const Vehicle* globalVehicles = globalEnv->getVehicles();
  Vehicle* noErrVehicles = noErrEnv.getVehicles();
  
  for (int i = 0; i < env->getNVehicles(); i++)
    for (int j = 0; j < globalEnv->getNVehicles(); j++)
      {
	if (globalVehicles[j].getID() != localVehicles[i].getID())
	  continue;

	noErrVehicles[i] = globalVehicles[j];
      }

  /* if you want errors place env instead of noErrEnv*/
  if (rMon)
    img.addAllVehicles(noErrEnv, observerID, monitorID, rMon->getReputation().level);
  
  img.writeFrameNumber();
 
  std::string fileName = img.save();

  addFrame(fileName.c_str());

  std::remove(fileName.c_str());
  delete hiddenObs;

}
