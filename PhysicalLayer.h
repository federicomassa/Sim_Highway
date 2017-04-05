#ifndef PHYSICALLAYER_H
#define PHYSICALLAYER_H

#include "Logger.h"
#include "systemParms.h"
#include "systemTypes.h"

extern Logger LOG;

class PhysicalLayer
{
    State q;
    Parms p;
    State nextQ;
    int idx;
    void (*controlFuncPointer)(const State&, State&, const int&, const Parms&, const Maneuver&, const List<Sensing>&, const bool&);
public:
    /* constructor */
    PhysicalLayer() { controlFuncPointer = 0;}
    /* destructor */
    ~PhysicalLayer() { }
    /* initialization */
    void init(const std::pair<State, Parms>&);
    /* evolution */
    void updateQ() { q = nextQ; }
    /* get state q */
    State getQ() const { return q; }
    /* get next state q */
    State getNextQ() const { return nextQ; }
    /* get parameters */
    Parms getParms() const { return p; }
    void setID(int index) {idx = index;}
    void setQ(const State& s) {q = s;}
    void computeNextQ(const Maneuver&, const List<Sensing>&, const bool& debug = false);
};

void unicycleControl(const State&, State&, const int&, const Parms&, const Maneuver&, const List<Sensing>&, const bool&);
void fastUnicycleControl(const State&, State&, const int&, const Parms&, const Maneuver&, const List<Sensing>&, const bool&);

#endif
