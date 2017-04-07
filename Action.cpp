#include "Action.h"
#include <iostream>
#include <sstream>

Action::Action()
{
  triggerTime = -1;
  endTime = -1;
  status = INACTIVE;
}

void Action::init(const Vector<Sensing, VEHICLE_MEMORY>& mStates, const Vector<List<Sensing>, VEHICLE_MEMORY>& nStates)
{
  monitorStates = &mStates;
  neighStates = &nStates;
  initRuleCategories();
}

Action::Action(const Action& a)
{
  triggerTime = a.triggerTime;
  endTime = a.endTime;
  monitorStates = a.monitorStates;
  status = a.status;
  ruleCategoryList = a.ruleCategoryList;
}

void Action::listen()
{

  /* Init condition */
  if (status == INACTIVE)
    {
      if (triggerCondition())
	{
	  status = TRIGGERED;
	  triggerTime = now - getTriggerOffset();
	  if (triggerTime < 0)
	    triggerTime = 0;
	}
    }

  /* Running */
  else if (status == TRIGGERED)
    {
      if (endCondition())
	{
	  status = ENDED;
	  endTime = now - getEndOffset();
	  if (endTime < 0)
	    endTime = 0;
	}
      
      else if (abortCondition())
	{
	  status = ABORTED;
	  endTime = now - getAbortOffset();
	}


      
    }

  /* Terminated */
  else
    {
      std::cout << "Take care of already terminated case FIXME" << std::endl;
    }
  
  //std::cout << toString(status) << '\t' << triggerTime << '\t' << endTime << std::endl;
  
}

void Action::print()
{
  std::cout << name() << '\t' << "from " << triggerTime << " to " << endTime << '\t' << toString(status) << std::endl;
}

std::string Action::info()
{
  std::string info;
  info = name() + '\t' + "from ";
  
  std::stringstream ss;
  std::string time;
  ss << triggerTime;
  ss >> time;
  ss.clear();
  
  info += time;
  
  ss << endTime;
  ss >> time;
  ss.clear();
  
  info += " to " + time + '\t' + toString(status);

  return info;
}

std::string toString(const ActionStatus& status)
{
  if (status == INACTIVE)
    return "INACTIVE";
  else if (status == TRIGGERED)
    return "TRIGGERED";
  else if (status == ENDED)
    return "ENDED";
  else if (status == ABORTED)
    return "ABORTED";

  return "";
}
