#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H

#include "controller.h"
#include "Logger.h"
#include "systemParms.h"
#include "systemTypes.h"

extern Logger LOG;

class Failure;
enum ControlFailure : int;

class PhysicalLayer
{
    State q;
    State nextQ;
    Parms maxV;
    int idx;
    //for mimic platoon
    bool tooClose;
    bool tooFar;

public:
    /* constructor */
    PhysicalLayer() { maxV = 0; }
    /* destructor */
    ~PhysicalLayer() { }
    /* initialization, parms is now dummy */
    void init(const State& s, const Parms& p = 0.0)
    {
        q = nextQ = s;
        // maxV = p; IT WAS LIKE THIS, BUT p is the initial speed, not the MAX_SPEED??? CHECK??
        maxV = MAX_SPEED;

        tooClose = true;
        //q.v = 0;
        //nextQ.v = 0;
    }
    /* evolution. If omniscient, Physical Layer of vehicle i is launched by vehicle i --> he knows everything, unlike when
    you launch it from monitor to make predictions */
    void computeNextQ(Maneuver sigma, /* For platoon  */List<State> qList, bool omniscient = true, bool debug = false);
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
