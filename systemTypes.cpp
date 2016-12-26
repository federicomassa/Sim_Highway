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
