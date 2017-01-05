
#ifndef FAILURE_H
#define FAILURE_H

#include <sstream>

#include "List.h"
#include "utility.h"

class Failure
{
public:
    /* automaton failure */
  enum AutomFailure { NORIGHT, JAMMER, NOPLATOON };
    /* communication failure */
    enum CommFailure { DUMMY };
    
    int idx;
    List<AutomFailure> aFailures;
    List<CommFailure> cFailures;
    
    Failure() { idx = -1; }
    Failure(int index) { idx = index; }
    Failure(string str);
    ~Failure() { }
};

ostream& operator<<(ostream& os, const Failure::AutomFailure& aF);

ostream& operator<<(ostream& os, const Failure::CommFailure& cF);

ostream& operator<<(ostream& os, const Failure& f);

bool operator==(const Failure& f1, const Failure& f2);

bool operator!=(const Failure& f1, const Failure& f2);

#endif
