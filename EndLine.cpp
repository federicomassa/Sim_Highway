
#include "EndLine.h"

using namespace std;
/* initialize indentation depth */
int EndLine::depth = 0;

EndLine::EndLine(Operation op)
{
  customDepth = -1;
  operation = op;
}

EndLine::EndLine(const int& d)
{
  customDepth = d;
}

ostream& operator<<(ostream& os, const EndLine& el)
{
  os << '\n';
  
  if (el.customDepth == -1)
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

      for(int i = 0; i < el.depth; i++)
	os << INDENTATION;
      
    }
  else
    {
      for(int i = 0; i < el.customDepth; i++)
	os << INDENTATION;
    }
  
  os.flush();
  return os;
}
