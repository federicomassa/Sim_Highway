
#ifndef ENDLINE_H
#define ENDLINE_H

#include <iostream>



#define INDENTATION "   "

class EndLine
{
  friend std::ostream& operator<<(std::ostream&, const EndLine&);
public:
    enum Operation { INC, DEC, NOP };
private:
    Operation operation;
    static int depth;
    int customDepth;
public:
    /* constructor */
    EndLine(Operation op = NOP);
    EndLine(const int&);
    /* destructor */
    ~EndLine() { }
};

#endif
