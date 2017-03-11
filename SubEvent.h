
#ifndef SUBEVENT_H
#define SUBEVENT_H

#include <string>

#include "Area.h"
#include "EndLine.h"
#include "ExtValue.h"
#include "List.h"
#include "Logger.h"
#include "systemTypes.h"
#include "utility.h"

extern Logger LOG;

/* time step */
extern int now;


class SubEvent
{
    friend class Monitor;
    friend std::ostream& operator<<(std::ostream&, const SubEvent&);
    int idx;
    bool (*func)(const State&, const IntVars&, const State&);
    void (*areaFunc)(const State&, Area&);
    EvalMode mode;
    ExtValue value;
    int evalTime;
    std::string name;
public:
    /* constructor */
    SubEvent() { idx = -1; }
    SubEvent(const SubEvent& se)
      {
	idx = se.idx;
	func = se.func;
	areaFunc = se.areaFunc;
	mode = se.mode;
	value = se.value;
	evalTime = se.evalTime;
	name = se.name;
      }
    /* destructor */
    ~SubEvent() { }
    /* initialization */
    void init(bool (*f)(const State&, const IntVars&, const State&), 
              void (*aF)(const State&, Area&), EvalMode m, const std::string n,
              int i);
    /* reset SubEvent evaluation time */
    void reset() { evalTime = -1; }
    /* evaluate sub event */
    void evaluate(const State&, const IntVars&,
                  const List<State>&);
    /* return Object's idx */
    int getID() const { return idx; }
    /* return Object's value */
    ExtValue& getValue() { return value; }

    /* get mode */
    EvalMode getMode() const {return mode;}
    
    /* areaFunc availability */
    bool hasArea() const {return (areaFunc != NULL);}
    
    /* evaluate area func */
    void evaluateArea(const State& q, Area& a) const
    {areaFunc(q, a);}
};

std::ostream& operator<<(std::ostream&, const SubEvent*);

#endif
