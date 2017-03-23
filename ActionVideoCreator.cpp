#include "ActionVideoCreator.h"
#include "utility.h"
#include "State.h"
#include "Image.h"
#include "Environment.h"
#include "actionParms.h"
#include "Actions.h"

#include <gd.h>
#include <cstdio>
#include <string>

ActionVideoCreator::ActionVideoCreator(const int& obsID, const int& monID) : observerID(obsID), monitorID(monID)
{
  leftFound = false;
  leftEnded = false;
}

/* This method creates a frame of the video */
void ActionVideoCreator::run()
{
  /* error handling */
  if (!isInitialized())
    error("ActionVideoCreator::run", "Video is not initialized");

  if (env == NULL)
    error("ActionVideoCreator::run", "Environment is not initialized");

  Vehicle* monitorV = env->getVehicleFromID(monitorID);
  if (monitorV == NULL)
    error("ActionVideoCreator::run", "Monitor vehicle's ID not found");

  Vehicle* observerV = env->getVehicleFromID(observerID);
  if (observerV == NULL)
    error("ActionVideoCreator::run", "Observer vehicle's ID not found");


  /* Get observer's ActionManager */
  List<const Action*> activeActions = observerV->getMonitorLayer()->getMonitor(monitorID)->getActionManager()->getActiveActions();
  Iterator<const Action*> aIt(activeActions);
  const Action* tmpA;

  bool isLeftPresent = false;
  
  while (aIt(tmpA))
    {
      if (dynamic_cast<const LeftAction*>(tmpA))
	{
	  if (!leftFound)
	    {
	      leftFound = true;
	      leftTriggerTime = now;
	      leftTriggerState = monitorV->getQ();
	    }
	  isLeftPresent = true;
	}
    }
  
  if (leftFound && !isLeftPresent && !leftEnded)
    {
      leftEnded = true;
      leftEndTime = now;
      leftEndState = monitorV->getQ();
    }

  /* save left trajectory */
  if (leftFound && !leftEnded)
    {
      monitorTrajectory.insHead(monitorV->getQ());
    }
  
  Image img(monitorV->getQ().x);
  
  if (leftFound && !leftEnded)
    img.drawTrajectory(monitorTrajectory);

  const int& red = gdImageColorAllocate(img.getFrame(), 255, 0, 0);
  const int& green = gdImageColorAllocate(img.getFrame(), 0, 255, 0);
  //  const int& blue = gdImageColorAllocate(img.getFrame(), 0, 0, 255);

  if (floor(monitorV->getQ().y) == 0 && !leftFound)
    {
      /* draw left curvature radii */
      img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
		  floor(monitorV->getQ().y) + 1.5, R_MIN_TURN, 1, red);
      img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
		  floor(monitorV->getQ().y) + 1.5, R_MAX_TURN, 1, red);
      img.drawString("Left trigger condition: FALSE", red);
    }
  if (leftFound && !leftEnded)
    {
      img.drawArc(leftTriggerState.x, leftTriggerState.y,
		  floor(leftTriggerState.y) + 1.5, R_MIN_TURN, 1, green);
      img.drawArc(leftTriggerState.x, leftTriggerState.y,
		  floor(leftTriggerState.y) + 1.5, R_MAX_TURN, 1, green);
      img.drawArea(monitorV->getQ().x, 1.5 + Y_TOLERANCE,
		   FRAME_W/SCALE, 1.5 - Y_TOLERANCE, red);
      img.drawString("Left trigger condition: TRUE", green);
      img.drawString("Left end condition: FALSE", red, 1);
    }
  if (leftEnded)
    {
      img.drawArea(monitorV->getQ().x, 1.5 + Y_TOLERANCE,
		   FRAME_W/SCALE, 1.5 - Y_TOLERANCE, green);
      img.drawString("Left end condition: TRUE", green);
    }


  img.addAllVehicles(*env, observerID, monitorID);
  img.writeFrameNumber();

  
  /*  else if (floor(monitorV->getQ().y) == 1)
      {*/
      /* draw right curvature radii */
  /*      img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
		  floor(monitorV->getQ().y) - 0.5, R_MIN_TURN, -1, red);
      img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
		  floor(monitorV->getQ().y) - 0.5, R_MAX_TURN, -1, red);
		  }*/
   
  /* draw left and right travel curvature radii */
  /*  img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
	      floor(monitorV->getQ().y) + 1.5, R_MIN_TRAVEL, 1, blue);
  img.drawArc(monitorV->getQ().x, monitorV->getQ().y,
  floor(monitorV->getQ().y) - 0.5, R_MIN_TRAVEL, -1, blue);*/

  
  std::string fileName = img.save();

  addFrame(fileName.c_str());

  std::remove(fileName.c_str());
}
