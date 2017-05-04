
#ifndef EVENT_H
#define EVENT_H

#include "SubEvent.h"
#include "Sensing.h"

class Event
{
    friend class Monitor;
    friend std::ostream& operator<<(std::ostream&, const Event&);
    friend bool operator==(const Event&, const Event&);
    friend bool operator!=(const Event&, const Event&);
    int idx;
    List<SubEvent*> subEventList;
    ExtValue value;
public:
    /* constructor */
    Event() { idx = -1; }
    /* destructor */
    ~Event() { }
    /* initialization */
    void init(const List<SubEvent*>& seList, int i)
    {
        subEventList = seList;
        idx = i;
    }
    /* reset value */
    void reset() { value.reset(true); }
    /* evaluate the event */
    void evaluate(const Sensing& sSubj, const IntVars& vars,
                  const List<Sensing>& sList, bool omniscient);

    /* Evaluate events using the observable area */
    void evaluateWithArea(const Sensing& sSubj, const IntVars& vars,
			  const List<Sensing>& sList, bool omniscient, const Area& obs, List<Area>&, Area&);
    
    /* re-evaluate the event after some subEvents changed their value */    
    void reEvaluate();
    /* return Object's idx */
    int getID() const { return idx; }
    /* return Object's value */
    ExtValue getValue() const { return value; }

    /* Delete subevents objects */
    void deleteSubEvents();

    const List<SubEvent*>& getSubEventList() const {return subEventList;}
};

std::ostream& operator<<(std::ostream&, const Event*);

#endif
