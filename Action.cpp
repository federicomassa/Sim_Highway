#include "Action.h"
#include <iostream>
#include <sstream>

Action::Action()
{
  triggerTime = -1;
  endTime = -1;

  status = INACTIVE;
}

void Action::init(const Vector<State, 10>& mStates)
{
  monitorStates = &mStates;
  initBehaviours();
}

Action::Action(const Action& a)
{
  triggerTime = a.triggerTime;
  endTime = a.endTime;
  monitorStates = a.monitorStates;
  status = a.status;

  behaviourList = a.behaviourList;
}

void Action::listen()
{

  /* Init condition */
  if (status == INACTIVE)
    {
      if (triggerCondition())
	{
	  status = TRIGGERED;
	  triggerTime = now;
	}
    }

  /* Running */
  else if (status == TRIGGERED)
    {
      if (endCondition())
	{
	  status = ENDED;
	  endTime = now;
	}
      
      else if (abortCondition())
	{
	  status = ABORTED;
	  endTime = now;
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
