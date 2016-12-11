/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Reputation.cpp
 * DESCRIPTION: Implementation of the Reputation class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Reputation.h"

ostream& operator<<(ostream& os, const Reputation& r)
{
    os << "Reputation {" << EndLine(EndLine::INC);
    os << "TargetID: " << r.targetID << EndLine();
    os << "Target's state: " << r.qTarget << EndLine();
    os << "Reputation level: ";
    switch(r.level)
    {
        case CORRECT:
            os << "CORRECT";
            break;
        case FAULTY:
            os << "FAULTY";
            break;
        case UNCERTAIN:
            os << "UNCERTAIN";
            break;
        case UNSET:
            os << "UNSET";
            break;
    }
    os << EndLine();
    os << '}' << EndLine(EndLine::DEC);

    return os;
}

bool operator==(const Reputation& r1, const Reputation& r2)
{
    return r1.targetID == r2.targetID && r1.qTarget == r2.qTarget;
}

bool operator!=(const Reputation& r1, const Reputation& r2)
{
    return !(r1 == r2);
}
