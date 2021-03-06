
#ifndef SENSING_H
#define SENSING_H

#include "EndLine.h"
#include "systemTypes.h"
#include "State.h"

class Sensing
{
    friend int size(Sensing s)
    {
        return size(s.q.x) + size(s.q.y) + size(s.q.theta) + size(s.q.v);
    }

    friend std::ostream& operator<<(std::ostream& os, const Sensing& s);
    friend bool operator==(const Sensing& s1, const Sensing& s2);
    friend bool operator!=(const Sensing& s1, const Sensing& s2);
    friend bool operator<(const Sensing& s1, const Sensing& s2);
public:
    int agentID;
    State q;
    Maneuver sigma;
    Parms p;


    Sensing() { agentID = -1; }
    Sensing(int tIdx, const State& tQ, const Parms& tP, const Maneuver& tSigma)
    {
        init(tIdx, tQ, tP, tSigma);
    }

    void init(int tIdx, const State& tQ, const Parms& tP, const Maneuver& tSigma)
    {
        agentID = tIdx;
        q = tQ;
        p = tP;
        sigma = tSigma;
    }
    const Sensing& operator=(const Sensing& s)
    {
        agentID = s.agentID;
        q = s.q;
        p = s.p;
        sigma = s.sigma;

        return *this;
    }

};

#endif
