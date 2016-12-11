
#ifndef TRANSITION_H
#define TRANSITION_H

#include "Event.h"

class Transition
{
    friend class Monitor;
    friend ostream& operator<<(ostream&, const Transition&);
    List<Event*> eventList;
    ExtValue value;
    /* reset the value */
    void reset() { value.reset(false); }
public:
    /* constructor */
    Transition() { }
    /* destructor */
    ~Transition() { }
    /* initialization */
    void init(const List<Event*>& eList) { eventList = eList; }
    /* evaluate the event */
    void evaluate(const State& qSubj, const IntVars& vars,
                  const List<State>& qList, bool omniscient);
    /* re-evaluate the transition after some subEvents changed their value */
    void reEvaluate();
    /* return the value */
    ExtValue getValue() { return value; }
};

#endif
