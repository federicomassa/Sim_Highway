#include "ActionManager.h"
#include "LeftAction.h"
#include <iostream>


ActionManager::ActionManager()
{}

ActionManager::~ActionManager()
{
  clearListeners();
}

void ActionManager::init()
{
  addListener<LeftAction>();
}

void ActionManager::run()
{
  Iterator<Action*> iListener(listeners);
  Action* a;

  while (iListener(a))
    {
      /* Checks the conditions of this action. */
      a->listen();
      
      if (a->status == ENDED || a->status == ABORTED)
	{
	  history.insHead(a->info);
	  a->status = IDLE;
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
}

void ActionManager::clearListeners()
{
  Iterator<Action*> iListeners(listeners);
  Action* a;

  while (iListeners(a))
    {
      delete a;
    }
}
