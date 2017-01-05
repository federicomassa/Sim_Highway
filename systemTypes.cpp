#include "systemTypes.h"

ostream& operator<<(ostream& os, Maneuver m)
{
    switch(m)
    {
        case FAST:
            os << "FAST";
            break;
        case SLOW:
            os << "SLOW";
            break;
        case LEFT:
            os << "LEFT";
            break;
        case RIGHT:
            os << "RIGHT";
            break;
        case PLATOON:
      	    os << "PLATOON";
	    break;
        case UNKNOWN:
            os << "UNKNOWN";
            break;
    }
    
    return os;
}

ostream& operator<<(ostream& os, Control c)
{
    os << "Control (a = " << c.a << ", omega = " << c.omega << ')';
    return os;
}

string operator+(const string& str, Maneuver m)
{

  switch(m)
    {
    case FAST: return (str + "FAST");
    case SLOW: return (str + "SLOW");
    case LEFT: return (str + "LEFT");
    case RIGHT: return (str + "RIGHT");
    case PLATOON: return (str + "PLATOON");
    default: return (str + "UNKNOWN");
    }

  return "";
  
}

string operator+(Maneuver m, const string& str)
{
  return (str + m);
}

