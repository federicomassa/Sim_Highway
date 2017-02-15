
#include "Sensing.h"

using namespace std;

ostream& operator<<(ostream& os, const Sensing& s)
{
    os << "Sensing {" << EndLine(EndLine::INC);
    os << "AgentID: " << s.agentID << EndLine();
    os << "State: " << s.q << EndLine();
    os << "Parms: " << s.p << EndLine(EndLine::DEC);
    os << '}';

    return os;
}

bool operator==(const Sensing& s1, const Sensing& s2)
{
    return (s1.agentID == s2.agentID) && (s1.q == s2.q) && (s1.p == s2.p);
}

bool operator!=(const Sensing& s1, const Sensing& s2)
{
    return !(s1 == s2);
}

bool operator<(const Sensing& s1, const Sensing& s2)
{
  if (s1.agentID < s2.agentID)
    return true;
  else if (s1.agentID > s2.agentID)
    return false;
  else
    error("Sensing", "Found two measurements of same vehicle");

  return false;
}
