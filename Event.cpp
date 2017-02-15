
#include "Event.h"

using namespace std;

void Event::evaluate(const State& qSubj, const IntVars& vars,
                     const List<State>& qList, bool omniscient)
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
        tmpSE->evaluate(qSubj, vars, qList);
        if(CONF.debug)
            LOG.s << "Updating Event value " << value << " << ";
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

