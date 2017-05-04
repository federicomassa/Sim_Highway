#include "systemTypes.h"

using namespace std;
/* function to pass from std::string to maneuver */
Maneuver strToManeuver(const std::string& str)
{
  
  if (str == "FAST")
    return FAST;
  else if (str == "SLOW")
    return SLOW;
  else if (str == "LEFT")
    return LEFT;
  else if (str == "RIGHT")
    return RIGHT;
  else if (str == "PLATOON")
    return PLATOON;
  else
    return UNKNOWN;
  
}

/* function to pass from maneuver to std::string */
std::string maneuverToStr(const Maneuver& sigma)
{
  
  if (sigma == FAST)
    return "FAST";
  else if (sigma == SLOW)
    return "SLOW";
  else if (sigma == LEFT)
    return "LEFT";
  else if (sigma == RIGHT)
    return "RIGHT";
  else if (sigma == PLATOON)
    return "PLATOON";
  else
    return "UNKNOWN";
  
}

std::string maneuverToLowercase(const Maneuver& sigma)
{
  
  if (sigma == FAST)
    return "fast";
  else if (sigma == SLOW)
    return "slow";
  else if (sigma == LEFT)
    return "left";
  else if (sigma == RIGHT)
    return "right";
  else if (sigma == PLATOON)
    return "platoon";
  else
    return "unknown";
  
}




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

