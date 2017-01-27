#include <iostream>

#include "../Grid.h"
#include "../Tensor4.h"
#include "../List.h"
#include "../Sensing.h"
#include "../utility.h"

int main()
{

  List<Tensor4<Sensing> > list;
  Tensor4<Sensing> tens(2,2,2,2);

  Sensing s;

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++)
	for (int l = 0; l < 2; l++)
	  {
	    s.agentID = 1;
	    s.q = State(0.3 + 0.1*i, 1.8 + j , 10.3 + 10*k, (100.5 + 100*l)/210);
	    s.p = s.q.v;
	    s.sigma = FAST;

	    tens(i,j,k,l) = s;
	  }

  list.insHead(tens);
  
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      {
	if (i == j) continue;
	Grid g;
	g.drawStateSpace(list, i, j);
	g.save("PROVAGRID", toString(i) + "_" + toString(j));
      }
  
  return 0;
}
