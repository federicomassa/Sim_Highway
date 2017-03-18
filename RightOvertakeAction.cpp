#include "RightOvertakeAction.h"
#include "actionParms.h"
#include "utility.h"

#include <cmath>

const std::string RightOvertakeAction::actionName = "RIGHT_OVERTAKE";

RightOvertakeAction::RightOvertakeAction() {}
RightOvertakeAction::~RightOvertakeAction() {}

void RightOvertakeAction::initRuleCategories()
{
  addRuleCategory("RightOvertake");
  addRuleCategory("Safety");
}

bool RightOvertakeAction::triggerCondition()
{
  /* This action is triggered when the monitored vehicle is x-near another and the distance diminishes with time */
  if (monitorStates->length < 3)
    return false;
  
  /* if there is no other vehicle do not trigger */
  if ((*neighStates)[0].count() == 0)
    return false;

  /* list of candidates to overtake */
  List<Sensing> candidates;
  Sensing* s;
  
  for (int i = 0; i < (*neighStates)[2].count(); i++)
    {
      (*neighStates)[0].getElem(s, i);
      /* candidate must be in a previous lane and ahead of you (x-wise) */

      if (s->q.x > (*monitorStates)[2].x &&
	  floor(s->q.y) > floor((*monitorStates)[2].y))
	candidates.insHead(*s);
    }

  /* if more than one candidate, choose the closest one */
  double distance = 10000000;
  Sensing* target = 0;
  for (int i = 0; i < candidates.count(); i++)
    {
      candidates.getElem(s, i);
      if (pow(s->q.x - (*monitorStates)[2].x,2) + pow(s->q.y - (*monitorStates)[2].y,2) < distance)
	{
	  distance = pow(s->q.x - (*monitorStates)[2].x,2) + pow(s->q.y - (*monitorStates)[2].y,2);
	  target = s;
	}
    }

  /* if there is one suitable target */
  State* currTargetState = 0, *lastTargetState = 0, *previousTargetState = 0;
  if (target)
    {
      previousTargetState = &target->q;
      targetID = target->agentID;
    }
  else
    return false;

  /* now look in the neighStates for the older states */
  for (int i = 0; i < (*neighStates)[1].count(); i++)
    {
      (*neighStates)[1].getElem(s, i);
      if (s->agentID == targetID)
	{
	  /* target found */
	  lastTargetState = &s->q;
	}
    }

  for (int i = 0; i < (*neighStates)[0].count(); i++)
    {
      (*neighStates)[0].getElem(s, i);
      if (s->agentID == targetID)
	{
	  /* target found */
	  currTargetState = &s->q;
	}
    }

  if (currTargetState && lastTargetState && previousTargetState)
    {
      if (previousTargetState->x - (*monitorStates)[2].x < OVERTAKE_TRIGGER_DISTANCE)
	if (lastTargetState->x - (*monitorStates)[1].x < previousTargetState->x - (*monitorStates)[2].x)
	  if (currTargetState->x - (*monitorStates)[0].x < lastTargetState->x - (*monitorStates)[1].x)
	    return true;
    }
  
  return false;
}

bool RightOvertakeAction::endCondition()
{
  /* RightOvertakeAction is ended when the monitored has overtaken the target */
  Sensing* s = 0;
  State* currTargetState = 0;
  for (int i = 0; i < (*neighStates)[0].count(); i++)
    {
      (*neighStates)[0].getElem(s, i);
      if (s->agentID == targetID)
	{
	  /* target found */
	  currTargetState = &s->q;
	}
    }

  if (currTargetState)
    {
      if ((*monitorStates)[0].x > currTargetState->x + OVERTAKE_TRIGGER_DISTANCE)
	return true;
    }
  else
    return false;
   
  return false;

}

bool RightOvertakeAction::abortCondition()
{
  /* This action is aborted if the vehicle disappears from sight before ending the action */
  State* currTargetState = 0;
  Sensing* s = 0;
  for (int i = 0; i < (*neighStates)[0].count(); i++)
    {
      (*neighStates)[0].getElem(s, i);
      if (s->agentID == targetID)
	{
	  /* target found */
	  currTargetState = &s->q;
	}
    }

  if (currTargetState)
    return false;
  else
    return true;

  return false;
}
