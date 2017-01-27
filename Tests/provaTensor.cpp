#include "../Tensor4.h"
#include <iostream>

int main()
{
  
  Tensor4<double> t(2,2,2,2);
  Tensor4<double> t1(3,3,3,3);

  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k= 0; k < 2; k++)
	for (int l = 0; l < 2; l++)
	  t(i,j,k,l) = 1;

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k= 0; k < 3; k++)
	for (int l = 0; l < 3; l++)
	  t1(i,j,k,l) = 2;

  int cnt = 0;
  
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k= 0; k < 2; k++)
	for (int l = 0; l < 2; l++)
	  std::cout << t(i,j,k,l) << " " << ++cnt << std::endl;

  cnt = 0;
  
  t = t1;

  std::cout << std::endl;
  
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      for (int k= 0; k < 3; k++)
	for (int l = 0; l < 3; l++)
	  std::cout << t(i,j,k,l) << " " << ++cnt << std::endl;
  
  
  

      

  
  
  
  return 0;
}
