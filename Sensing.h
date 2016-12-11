
#ifndef SENSING_H
#define SENSING_H

#include "EndLine.h"
#include "systemTypes.h"

class Sensing
{
    friend ostream& operator<<(ostream& os, const Sensing& s);
    friend bool operator==(const Sensing& s1, const Sensing& s2);
    friend bool operator!=(const Sensing& s1, const Sensing& s2);
public:
    int agentID;
    State q;
    Parms p;
    
    Sensing() { agentID = -1; }
    Sensing(int tIdx, const State& tQ, const Parms& tP)
    {
        agentID = tIdx;
        q = tQ;
        p = tP;
    }
};

#endif
