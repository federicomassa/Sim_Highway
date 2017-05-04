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
      os << "Target's sensing: " << n.getTargetSensing() << EndLine();
      os << "Sensing list: " << n.getSList() << EndLine();
      os << '}';
            
    };
  return os;

}
