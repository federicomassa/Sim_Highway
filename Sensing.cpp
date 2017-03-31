#include "State.h"
#include "Sensing.h"

using namespace std;

ostream& operator<<(ostream& os, const Sensing& s)
{
    os << "Sensing {" << EndLine(EndLine::INC);
    os << "AgentID: " << s.agentID << EndLine();
    os << "State: { " << "x = " << s.x << EndLine();
    os << "         y = " << s.y << EndLine();
    os << "         theta = " << s.theta << EndLine();
    os << "         v = " << s.v << " }" << EndLine();
    os << "Parms: { " << "desiredV = " << s.desiredV << EndLine();
    os << "         vehicleType = " << s.vehicleType.getName()  << " }" << EndLine(EndLine::DEC);
    os << '}';
    os.flush();
    
    return os;
}

bool operator==(const Sensing& s1, const Sensing& s2)
{
    return (s1.agentID == s2.agentID) && (s1.x == s2.x) &&
      (s1.y == s2.y) &&
      (s1.theta == s2.theta) &&
      (s1.v == s2.v) &&
      (s1.desiredV == s2.desiredV) &&
      (s1.vehicleType == s2.vehicleType);
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
