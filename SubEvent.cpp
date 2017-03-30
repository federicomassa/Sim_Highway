
#include "SubEvent.h"
#include "Sensing.h"

using namespace std;

void SubEvent::init(bool (*f)(const Sensing&, const IntVars&, const Sensing&),
          void (*aF)(const Sensing&, Area&), EvalMode m, const string n,
          int i)
{
    func = f;
    areaFunc = aF;
    mode = m;
    name = n;
    idx = i;
    reset();
}

void SubEvent::evaluate(const Sensing& sSubj, const IntVars& vars,
                        const List<Sensing>& sList)
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
        LOG.s << "Subject state = " << sSubj << EndLine();
        LOG.s << "Internal vars = " << vars;
    }
    
    if(mode == SINGLE || mode == NSINGLE)
    {
        if(CONF.debug)
            LOG.s << EndLine() << "Evaluating single function: returnValue << ";
        Sensing voidS;
        value.omniscientValue = func(sSubj, vars, voidS);
        if(CONF.debug)
            LOG.s << value.omniscientValue;
    }
    else
    {
        /* check if the list is empty */
        if(!sList.isEmpty())
        {
            Iterator<Sensing> si(sList);
            Sensing tmpS;
            while(si(tmpS))
            {
                if(CONF.debug)
                {
                    LOG.s << EndLine() << "Evaluating function for state ";
                    LOG.s << tmpS << ": returnValue << ";
                }
                value.omniscientValue =
                    value.omniscientValue || func(sSubj, vars, tmpS);
                if(CONF.debug)
                    LOG.s << value.omniscientValue;
            }
        }
        else
        {

	  /* FIXME: if nobody else around monitor, OR subevents are automatically false, NOR are true */
            if(CONF.debug)
                LOG.s << EndLine() << "No other states: omniscentValue = false";
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
