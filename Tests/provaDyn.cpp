#include "../DynVector.h"

#include <iostream>

int main()
{
  DynVector<double> v(2);

  std::cout << "After v" << std::endl;
  
  DynVector<double> w(5);

  std::cout << "After w" << std::endl;
  
  for (int i = 0; i < 5; i++)
    w[i] = 1.1*i;

  std::cout << "w set" << std::endl;

  v[0] = 4.4;
  v[1] = 3.3;

  std::cout << "v set" << std::endl;
  
   for (int i = 0; i < 5; i++)
     std::cout << w[i] << std::endl;

   //   v.insert(7.7);

   std::cout << v.length << '\t' << w.length << std::endl;
  
  return 0;
}
