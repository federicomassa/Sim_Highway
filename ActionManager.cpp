#include "ActionManager.h"
#include "Actions.h"
#include <iostream>
#include <string>
#include <cstdlib>

ActionManager::ActionManager()
{}

ActionManager::~ActionManager()
{
  /* Take care of memory management */
  clearListeners();
}

void ActionManager::init()
{
  /* Add here any action that you want to be listened to. */
  addListener<LeftAction>();
  addListener<RightAction>();
  addListener<TravelAction>();
}

void ActionManager::run()
{
  Iterator<Action*> iListener(listeners);
  Action* a;

  while (iListener(a))
    {
      /* Checks the conditions of this action and update its status. */
      a->listen();
      
      if (a->status == ENDED || a->status == ABORTED || (a->status == TRIGGERED && now == CONF.nSteps -1))
	{
	  /* Add to history and reset action */
	  if (now == CONF.nSteps -1)
	    a->endTime = now;
	  
	  history.insHead(a->info());
	  a->status = INACTIVE;
	  a->triggerTime = -1;
	  a->endTime = -1;
	}
    }
  
}

void ActionManager::printHistory()
{
  Iterator<std::string> iHistory(history);
  std::string s;
  
  while (iHistory(s))
    {
      std::cout << s << std::endl;
    }

  /*  double cX, cY, R;
  circle3points(1, 0, 0, 1, -1, 0, cX, cY, R);

  std::cout << "Circle: " << cX << '\t' << cY << '\t' << R << std::endl;*/
  
}

void ActionManager::clearListeners()
{
  Iterator<Action*> iListeners(listeners);
  Action* a;

  while (iListeners(a))
    {
      delete a;
    }

  listeners.purge();
}

void ActionManager::parseTestFile(std::ifstream& input)
{
  std::string line;

  if (!input.eof())
    {
      double x, y, theta, v;
      
      getline(input, line, ' ');
      x = atof(line.c_str());

      getline(input, line, ' ');
      y = atof(line.c_str());

      getline(input, line, ' ');
      theta = atof(line.c_str());

      getline(input, line, '\n');
      v = atof(line.c_str());
      
      State q(x, y, theta, v, 0);

      /*      std::cout << q << std::endl;*/
      
      monitorStates.insHead(q);
    }
  
}
