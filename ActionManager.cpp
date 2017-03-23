#include "ActionManager.h"
#include "Actions.h"
#include <iostream>
#include <string>
#include <cstdlib>

ActionManager::ActionManager(const Vector<State, 10>& mStates, const Vector<List<Sensing>, 10>& nStates) : monitorStates(mStates),
													 neighStates(nStates)
{}

ActionManager::~ActionManager()
{
  /* Take care of memory management */
  clearListeners();
  clearHistory();
}

void ActionManager::init()
{
  /* Add here any action that you want to be listened to. */
  addListener(new LeftAction);
  addListener(new RightAction);
  addListener(new TravelAction);
  addListener(new LeftOvertakeAction);
  addListener(new RightOvertakeAction);
}

void ActionManager::run()
{
  Iterator<Action*> iListener(listeners);
  Action* a;

  while (iListener(a))
    {
      /* Checks the conditions of this action and update its status. */
      a->listen();
      
      if (a->status != INACTIVE)
	{
	  /* Add to history and reset action */

	  recordAction(a);

	  if (a->status != TRIGGERED)
	    resetAction(a);
	  
	}
    }
  
}

void ActionManager::resetAction(Action* a)
{
  a->status = INACTIVE;
  a->triggerTime = -1;
  a->endTime = -1;
}

void ActionManager::addListener(Action* a)
{
  a->init(monitorStates, neighStates);
  listeners.insHead(a);
}

Action* ActionManager::copyAction(Action* a)
{
  Action* copy = 0;
  
  if (a->name() == LeftAction::actionName)
    copy = new LeftAction(*a);
  else if (a->name() == RightAction::actionName)
    copy = new RightAction(*a);
  else if (a->name() == TravelAction::actionName)
    copy = new TravelAction(*a);
  else if (a->name() == LeftOvertakeAction::actionName)
    copy = new LeftOvertakeAction(*a);
  else if (a->name() == RightOvertakeAction::actionName)
    copy = new RightOvertakeAction(*a);

  else
    error("ActionManager::copyAction", "Forgot to add new action to record list? Action name: " + a->name());
  
  return copy;

}

Action* ActionManager::copyAction(const Action* a)
{
  Action* copy = 0;
  
  if (a->name() == LeftAction::actionName)
    copy = new LeftAction(*a);
  else if (a->name() == RightAction::actionName)
    copy = new RightAction(*a);
  else if (a->name() == TravelAction::actionName)
    copy = new TravelAction(*a);
  else if (a->name() == LeftOvertakeAction::actionName)
    copy = new LeftOvertakeAction(*a);
  else if (a->name() == RightOvertakeAction::actionName)
    copy = new RightOvertakeAction(*a);
 
  else
    error("ActionManager::copyAction", "Forgot to add new action to record list? Action name: " + a->name());
  
  return copy;

}


void ActionManager::recordAction(Action* a)
{

  Iterator<Action*> iHistory(history);
  Action* tmpA;
  Action* foundAction = 0;
  
  bool saveAction = false;
  
  while (iHistory(tmpA))
    {
      if (tmpA->name() == a->name() &&
	  tmpA->triggerTime == a->triggerTime)
	foundAction = tmpA;

      /* recent actions are in the head of the history list. This stops the search if the current entry is too old. */
      if (tmpA->triggerTime < a->triggerTime)
	break;
    }

  if (foundAction == NULL)
    {
      saveAction = true;
    }
  else
    {
      /* Status changed? Needs update */
      if (a->status != foundAction->status)
	{
	  foundAction->status = a->status;
	  foundAction->endTime = a->endTime;
	}
    }


  
  if (saveAction)
    history.insHead(copyAction(a));

  
  
}

void ActionManager::printHistory()
{
  Iterator<Action*> iHistory(history);
  Action* a;
  
  while (iHistory(a))
    {
      ResultLog.s << a->info() << EndLine();
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
      if (a)
	delete a;
    }

  listeners.purge();
}

void ActionManager::clearHistory()
{
  Iterator<Action*> iHistory(history);
  Action* a;

  while (iHistory(a))
    {
      if (a)
	delete a;
    }

  history.purge();
}


void ActionManager::parseTestFile(std::ifstream& input)
{
  /*
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

      
      monitorStates.insHead(q);
    }
  */
}

List<const Action*> ActionManager::getActiveActions() const
{
  List<const Action*> activeList;
  Iterator<Action*> historyIt(history);
  Action* tmpA;

  while (historyIt(tmpA))
    {
      if (tmpA->status == TRIGGERED)
	activeList.insHead(tmpA);
    }
  
  return activeList;
}
