
#include "EndLine.h"

/* initialize indentation depth */
int EndLine::depth = 0;

EndLine::EndLine(Operation op)
{
    operation = op;
}

ostream& operator<<(ostream& os, const EndLine& el)
{
    switch(el.operation)
    {
        case EndLine::INC:
            el.depth++;
            break;
        case EndLine::DEC:
            el.depth--;
            break;
        default:
            // NOP...
            break;
    }
    os << '\n';
    for(int i = 0; i < el.depth; i++)
        os << INDENTATION;
    os.flush();
    return os;
}
