
#include "Event.h"

using namespace std;

void Event::evaluate(const Sensing& sSubj, const IntVars& vars,
                     const List<Sensing>& sList, bool omniscient)
{
    /* error handling */
    if(idx == -1)
        error("Event", "evaluate uninizialized Event (ID = -1)");
    
    if(CONF.debug)
    {
        LOG.s << "Evaluating Event " << idx;
        LOG.s << " BEGIN..." << EndLine(EndLine::INC);
    }
    reset();
    if(CONF.debug)
    {
        LOG.s << "Resetting value: " << value << EndLine();
        LOG.s << "now = " << now;
    }

    Iterator<SubEvent*> seIt(subEventList);
    SubEvent* tmpSE;
    while (seIt(tmpSE))
    {
        if(CONF.debug)
        {
            LOG.s << EndLine() << "Calling evaluation of SubEvent ";
            LOG.s << tmpSE->getID() << "..." << EndLine();
        }
        tmpSE->evaluate(sSubj, vars, sList);
        if(CONF.debug)
            LOG.s << "Updating Event value " << value << " << ";

	/* event value is the AND of the subevent values */
        value = value && tmpSE->getValue();
        if(CONF.debug)
            LOG.s << value;

        /* when value cannot change any more, iteration can be stopped */
        if(omniscient) {
            /* omniscient case */
	  if(value.omniscientValue == false)
	    break;
	
	  else {
	    /* non-omniscient case */
	    if(value.nonOmniscientValue == F)
	      break;
	  }
	}
    }

    if(CONF.debug)
      {
        LOG.s << EndLine(EndLine::DEC) << "Evaluating Event ";
        LOG.s << idx;
        LOG.s << " END:" << value << EndLine();
      }
}

void Event::evaluateWithArea(const Sensing& sSubj, const IntVars& vars,
			     const List<Sensing>& sList, bool omniscient, const Area& obs,
			     List<Area>& positiveArea, Area& negativeArea)
{
  /* First normal evaluation */  
  evaluate(sSubj, vars, sList, omniscient);

  /* true if some subevents have changed value next to the area evaluation */
  bool valueChanged = false;
  
  Iterator<SubEvent*> seIt(subEventList);
  SubEvent* se;

  /* Now let's see if our observable area is sufficient to draw some conclusions... */
  while (seIt(se))
    {
      /* If we are already certain there is no reason to do this*/
      if (se->getValue().nonOmniscientValue == U)
	{
	  /* if it has an associated area */
	  if(se->hasArea())
	    {
	      /* compute sub-event indicator area */
	      Area indicator;
	      se->evaluateArea(sSubj, indicator);
	      /* subtract observable area */
	      indicator -= obs;
	      if(indicator.isEmpty())
		{
		  if(se->getMode() == OR)
		    /* there isn't any agent where there should be */
		    se->getValue().nonOmniscientValue = F;
		  else
		    /* there isn't any agent where there shouldn't be */
		    se->getValue().nonOmniscientValue = T;
		  valueChanged = true;
		  if(CONF.debug)
		    LOG.s << EndLine() << "Value changed: " << se->getValue();
		}
	      else
		{
		  /* positive areas do not sum, in order for the event to be true,
		   every OR subevent must contain a vehicle. In NOR subevents,
		  instead, there mustn't be anyone in the whole area */
		  if(se->getMode() == OR)
		    positiveArea.insTail(indicator);
		  else
		    negativeArea += indicator;
		}
	    }
	} 
    }

  if (valueChanged)
    reEvaluate();
  
}


void Event::reEvaluate()
{
    /* error handling */
    if(idx == -1)
        error("Event", "evaluate uninizialized Event (ID = -1)");
    
    if(CONF.debug)
    {
        LOG.s << "Re-evaluating Event " << idx;
        LOG.s << " BEGIN..." << EndLine(EndLine::INC);
    }
    reset();
    if(CONF.debug)
    {
        LOG.s << "Resetting value: " << value << EndLine();
        LOG.s << "now = " << now;
    }

    Iterator<SubEvent*> seIt(subEventList);
    SubEvent* tmpSE;
    while (seIt(tmpSE))
    {
        if(CONF.debug)
        {
            LOG.s << EndLine() << "SubEvent ";
            LOG.s << tmpSE->getID() << "..." << EndLine();
            LOG.s << "Updating Event value " << value << " << ";
        }
        value = value && tmpSE->getValue();
        if(CONF.debug)
            LOG.s << value;

        /* in this case Event value won't change any more */
        if(value.nonOmniscientValue == F)
            break;
    }
    
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC) << "Re-evaluating Event ";
        LOG.s << idx;
        LOG.s << " END:" << value << EndLine();
    }
}

void Event::deleteSubEvents()
{
  Iterator<SubEvent*> seIt(subEventList);
  SubEvent* tmpSE;

  while (seIt(tmpSE))
    {
      if (tmpSE)
	delete tmpSE;
    }

  subEventList.purge();
}

ostream& operator<<(ostream& os, const Event& e)
{
    os << "Event {" << EndLine(EndLine::INC);
    os << "ID: ";
    os.fill('0');
    os.width(2);
    os << e.idx;
    os << EndLine();
    os << "SubEvents: " << e.subEventList << EndLine(EndLine::DEC);
    os << '}';
    return os;
}

ostream& operator<<(ostream& os, const Event* e)
{
    os << *e;
    return os;
}

bool operator==(const Event& e1, const Event& e2)
{
    return e1.idx == e2.idx;
}

bool operator!=(const Event& e1, const Event& e2)
{
    return e1.idx != e2.idx;
}

