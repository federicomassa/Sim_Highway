
#include "../List.h"
#include "../DynVector.h"
#include "../Tensor5.h"
#include "../Vector.h"
#include "../Sensing.h"

#include <iostream>

int main()
{
  State q(1.1,2.2,0,0.5,0.54,"FAST");
  Sensing s(0, q, q.v, FAST);
  
  Vector<List<Tensor5<Sensing> >, 3> v;
  List<Tensor5<Sensing> > l;
  Tensor5<Sensing> tens(1,1,1,1,1);
  for (int i = 0; i < 1; i++)
    for (int j = 0; j < 1; j++)
      for (int k = 0; k < 1; k++)
	for (int l = 0; l < 1; l++)
	  for (int m = 0; m < 1; m++)
	    tens(i,j,k,l,m) = s;
  
  l.insHead(tens);
  l.insHead(tens);
  l.insHead(tens);

  Tensor5<Sensing> tmpTens;
  std::cout << "Before extrHead" << std::endl;
  l.extrHead(tmpTens);
  std::cout << "After extrHead" << std::endl;


  std::cout << "List count: " << l.count() << std::endl;
  
  v[0] = l;
  std::cout << "Filled vector 0" << std::endl;
  v[1] = l;
  v[2] = l;

  std::cout << "Decl" << std::endl;
  
  Vector<List<Tensor5<Sensing> >, 3> w;

  std::cout << "Before = " << std::endl;
  w = v;
  std::cout << "After = " << std::endl;

  
  
  return 0;
}
