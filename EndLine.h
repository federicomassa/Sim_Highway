
#ifndef ENDLINE_H
#define ENDLINE_H

#include <iostream>

using namespace std;

#define INDENTATION "   "

class EndLine
{
    friend ostream& operator<<(ostream&, const EndLine&);
public:
    enum Operation { INC, DEC, NOP };
private:
    Operation operation;
    static int depth;
public:
    /* constructor */
    EndLine(Operation op = NOP);
    /* destructor */
    ~EndLine() { }
};

#endif
