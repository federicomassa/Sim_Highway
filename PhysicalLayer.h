
#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H

#include "controller.h"
#include "Logger.h"
#include "systemParms.h"
#include "systemTypes.h"

extern Logger LOG;

class PhysicalLayer
{
    State q;
    State nextQ;
    Parms maxV;
    int idx;
public:
    /* constructor */
    PhysicalLayer() { maxV = 0; }
    /* destructor */
    ~PhysicalLayer() { }
    /* initialization */
    void init(const State& s, const Parms& p)
    {
        q = nextQ = s;
        // maxV = p; IT WAS LIKE THIS, BUT p is the initial speed, not the MAX_SPEED??? CHECK??
	maxV = MAX_SPEED;
        //q.v = 0;
        //nextQ.v = 0;
    }
    /* evolution */
    void computeNextQ(Maneuver sigma, /* For platoon  */List<State> qList);
    void updateQ() { q = nextQ; }
    /* get state q */
    State getQ() const { return q; }
    /* get next state q */
    State getNextQ() const { return nextQ; }
    /* get parameters */
    Parms getParms() const { return maxV; }
    void setID(int index) {idx = index;}
};

#endif
