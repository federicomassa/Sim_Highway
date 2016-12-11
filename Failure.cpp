
#include "Failure.h"

Failure::Failure(string str)
{
    stringstream token(str);
    token >> idx;
    string tmpS;
    for(token >> tmpS; (int)token.tellg() != -1; token >> tmpS)
    {
        /* automaton failures */
        if(tmpS == "NORIGHT")
        {
            aFailures.insHead(NORIGHT, true);
            continue;
        }
        if(tmpS == "JAMMER")
        {
            aFailures.insHead(JAMMER, true);
            continue;
        }
        /* communication failures */
        if(tmpS == "DUMMY")
        {
            cFailures.insHead(DUMMY, true);
            continue;
        }
        /* error handling */
        error("Failure", "parse error (" + tmpS + ')');
    }    
}

ostream& operator<<(ostream& os, const Failure::AutomFailure& aF)
{
    switch(aF)
    {
        case Failure::NORIGHT:
            os << "NORIGHT";
            break;
        case Failure::JAMMER:
            os << "JAMMER";
            break;
    }
    
    return os;
}

ostream& operator<<(ostream& os, const Failure::CommFailure& cF)
{
    switch(cF)
    {
        case Failure::DUMMY:
            os << "DUMMY";
            break;
    }
    
    return os;
}    

ostream& operator<<(ostream& os, const Failure& f)
{
    os << "Failure {" << EndLine(EndLine::INC);
    os << "Agent ID: " << f.idx << EndLine();
    os << "Automaton failures: " << f.aFailures << EndLine();
    os << "Communication failures: " << f.cFailures << EndLine(EndLine::DEC);
    os << '}';
    
    return os;
}

bool operator==(const Failure& f1, const Failure& f2)
{
    return f1.idx == f2.idx;
}

bool operator!=(const Failure& f1, const Failure& f2)
{
    return !(f1 == f2);
}
