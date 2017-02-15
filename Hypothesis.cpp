
#include "Hypothesis.h"

using namespace std;

double Hypothesis::measure() const
{
    double mu = negative.measure();
    Iterator<SubHypothesis> i(subHypList);
    SubHypothesis sH;
    while(i(sH))
        mu += sH.measure();
    
    return mu;
}

ostream& operator<<(ostream& os, const Hypothesis& h)
{
    os << "Hypothesis {" << EndLine(EndLine::INC);
    os << "Event ID: " << h.eventID << EndLine();
    os << "SubHypothesis list: " << h.subHypList << EndLine();
    os << "Negative area: " << h.negative << EndLine(EndLine::DEC);
    os << '}';
    
    return os;
}

ostream& operator<<(ostream& os, const Hypothesis::SubHypothesis& sh)
{
    os << "SubHypothesis {" << EndLine(EndLine::INC);
    os << "SubEvent ID: " << sh.subEventID << EndLine();
    os << "Positive area: " << sh.positive << EndLine(EndLine::DEC);
    os << '}';
    
    return os;
}
