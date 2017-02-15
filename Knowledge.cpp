#include "Knowledge.h"

using namespace std;

Knowledge::Knowledge() {}

ostream& operator<<(ostream& os, const Knowledge& k)
{
  Iterator<Neighborhood> iterN(k.nList);

  Neighborhood n;
  pair<int, int> p;
  
  while (iterN(n))
    {
      os << "Neighborhood {" << EndLine(EndLine::INC);
      os << "Target ID: " << n.getTargetID() << EndLine();
      os << "Target's state: " << n.getTargetState() << EndLine();
      os << "State list: " << n.getQList() << EndLine();
      os << "Hypothesis list: " << n.getHList() << EndLine();
      os << "Measure (Mu): " << n.measure() << EndLine();
      os << '}';
      
      Iterator<pair<int, int> > iterP(k.monitoredVehicles);      
      while (iterP(p))
	{
	  if (p.first == n.getTargetID())
	    {
	      os << "Observer busy on this monitor for " << p.second << " steps. " << EndLine(EndLine::DEC);
	      break;
	    }
	  
	};
      
    };
  return os;

}
