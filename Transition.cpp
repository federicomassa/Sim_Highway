
#include "Transition.h"

using namespace std;

void Transition::evaluate(const State& qSubj, const IntVars& vars,
                          const List<State>& qList, bool omniscient)
{   
    if(CONF.debug)
        LOG.s << "Evaluating Transition BEGIN..." << EndLine(EndLine::INC);
    reset();
    if(CONF.debug)
    {
        LOG.s << "Resetting value: " << value << EndLine();
        LOG.s << "now = " << now;
    }

    Iterator<Event*> i(eventList);
    Event* tmpE;
    while (i(tmpE))
    {
        if(CONF.debug)
        {
            LOG.s << EndLine();
            LOG.s << "Calling evaluation of Event " << tmpE->getID() << "...";
            LOG.s << EndLine();
        }
        tmpE->evaluate(qSubj, vars, qList, omniscient);
        if(CONF.debug)
            LOG.s << "Updating Transition value " << value << " << ";
        value = value || tmpE->getValue();
        if(CONF.debug)
            LOG.s << value;
        
        /* when value cannot change any more, iteration can be stopped */
        if(omniscient) {
            /* omniscient case */
            if(value.omniscientValue == true)
                break;
	}
        else {
	  /* non-omniscient case */
	  if(value.nonOmniscientValue == T)
	    break;
	}
    }
    
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC) << "Evaluating Transition END: ";
        LOG.s << value << EndLine();
    }
}

void Transition::reEvaluate()
{   
    if(CONF.debug)
        LOG.s << "Re-evaluating Transition BEGIN..." << EndLine(EndLine::INC);
    reset();
    if(CONF.debug)
    {
        LOG.s << "Resetting value: " << value << EndLine();
        LOG.s << "now = " << now;
    }

    Iterator<Event*> i(eventList);
    Event* tmpE;
    while (i(tmpE))
    {
        if(CONF.debug)
        {
            LOG.s << EndLine();
            LOG.s << "Event " << tmpE->getID() << "...";
            LOG.s << EndLine();
        }
        tmpE->reEvaluate();
        if(CONF.debug)
            LOG.s << "Updating Transition value " << value << " << ";
        value = value || tmpE->getValue();
        if(CONF.debug)
            LOG.s << value;
        
        /* in this case Transition value won't change any more */
        if(value.nonOmniscientValue == T)
            break;
    }
    
    if(CONF.debug)
    {
        LOG.s << EndLine
	  (EndLine::DEC) << "Re-evaluating Transition END: ";
        LOG.s << value << EndLine();
    }
}

ostream& operator<<(ostream& os, const Transition& t)
{
    os << "Transition {" << EndLine(EndLine::INC);
    os << "Events: " << t.eventList << EndLine(EndLine::DEC);
    os << '}';
    return os;
}
