
#ifndef EVAL_MODE_H
#define EVAL_MODE_H

#include <iostream>

using namespace std;

// Evaluation mode
enum EvalMode { OR, NOR, SINGLE, NSINGLE };

ostream& operator<<(ostream&, EvalMode);

#endif
