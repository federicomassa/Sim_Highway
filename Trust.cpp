#include "Trust.h"

using namespace std;

ostream& operator<<(ostream& os, const Trust& t)
{
    os << "Trust (ID = " << t.agentID << ", global trust = " << t.global;
    os << ", local trust = " << t.local << ')';
    
    return os;
}

bool operator==(const Trust& t1, const Trust& t2)
{
    return t1.agentID == t2.agentID;
}

bool operator!=(const Trust& t1, const Trust& t2){
    return !(t1 == t2);
}
