
#include "SubEvent.h"

using namespace std;

void SubEvent::init(bool (*f)(const State&, const IntVars&, const State&),
          void (*aF)(const State&, Area&), EvalMode m, const string n,
          int i)
{
    func = f;
    areaFunc = aF;
    mode = m;
    name = n;
    idx = i;
    reset();
}

void SubEvent::evaluate(const State& qSubj, const IntVars& vars,
                        const List<State>& qList)
{
    /* error handling */
    if(idx == -1)
        error("SubEvent", "evaluate uninizialized SubEvent (ID = -1)");
    
    /* check if cached value is valid */
    if(evalTime == now)
    {
        if(CONF.debug)
        {
            LOG.s << "Evaluating " << *this;
            LOG.s << " BEGIN..." << EndLine(EndLine::INC);
            LOG.s << "Using cached value: " << value << EndLine(EndLine::DEC);
            LOG.s << "Evaluating SubEvent END." << EndLine();
        }
        return;
    }
    
    evalTime = now;
    
    if(CONF.debug)
    {
        LOG.s << "Evaluating " << *this;
        LOG.s << " BEGIN..." << EndLine(EndLine::INC);
    }
    value.reset();
    if(CONF.debug)
    {
        LOG.s << "Resetting value: " << value << EndLine();
        LOG.s << "now = " << now << EndLine();
        LOG.s << "Evaluation mode = " << mode << EndLine();
        LOG.s << "Subject state = " << qSubj << EndLine();
        LOG.s << "Internal vars = " << vars;
    }
    
    if(mode == SINGLE || mode == NSINGLE)
    {
        if(CONF.debug)
            LOG.s << EndLine() << "Evaluating single function: returnValue << ";
        State voidS;
        value.omniscientValue = func(qSubj, vars, voidS);
        if(CONF.debug)
            LOG.s << value.omniscientValue;
    }
    else
    {
        /* check if the list is empty */
        if(!qList.isEmpty())
        {
            Iterator<State> qi(qList);
            State tmpQ;
            while(qi(tmpQ))
            {
                if(CONF.debug)
                {
                    LOG.s << EndLine() << "Evaluating function for state ";
                    LOG.s << tmpQ << ": returnValue << ";
                }
                value.omniscientValue =
                    value.omniscientValue || func(qSubj, vars, tmpQ);
                if(CONF.debug)
                    LOG.s << value.omniscientValue;
            }
        }
        else
        {
            if(CONF.debug)
                LOG.s << EndLine() << "No other states: onmiscentValue = false";
            value.omniscientValue = false;
        }
    }
    if(mode == NOR || mode == NSINGLE)
    {
        if(CONF.debug)
        {
            LOG.s << EndLine();
            LOG.s << "mode is NOR or NSINGLE: omniscientValue = ";
        }
        value.omniscientValue = !value.omniscientValue;
        if(CONF.debug)
            LOG.s << value.omniscientValue;
    }
    value.extendValue(mode);
    if(CONF.debug)
    {
        LOG.s << EndLine(EndLine::DEC) << "Evaluating SubEvent END: ";
        LOG.s << value << EndLine();
    }
}

ostream& operator<<(ostream& os, const SubEvent& s)
{
    os << "SubEvent (";
    os.fill('0');
    os.width(2);
    os << s.idx << " - " << s.name << ')';
    return os;
}

ostream& operator<<(ostream& os, const SubEvent* s)
{
    os << *s;
    return os;
}
