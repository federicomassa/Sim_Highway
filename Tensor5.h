#ifndef TENSOR5_H
#define TENSOR5_H

#include "DynVector.h"
#include "utility.h"

#include <iostream>

template<typename T>
class Tensor5
{
  DynVector<DynVector<DynVector<DynVector<DynVector<T> > > > > t;
  
 public:
  int Dim1;
  int Dim2;
  int Dim3;
  int Dim4;
  int Dim5;

  // Constructor with tensor dimensions
  Tensor5(const int&,
	  const int&,
	  const int&,
	  const int&,
	  const int&);

  // Default constructor --> t has to be initialized
  Tensor5() {Dim1 = 0; Dim2 = 0; Dim3 = 0; Dim4 = 0; Dim5 = 0;};

  // Copy constructor
  Tensor5(const Tensor5& t1) {
    Dim1 = t1.Dim1;
    Dim2 = t1.Dim2;
    Dim3 = t1.Dim3;
    Dim4 = t1.Dim4;
    Dim5 = t1.Dim5;
    t = t1;}
  void init(const int&,
	    const int&,
	    const int&,
	    const int&,
	    const int&);
  
  
  
  T& operator()(const int& n1, const int& n2, const int& n3, const int& n4, const int& n5)
    {
      return t[n1][n2][n3][n4][n5];
    }

  const T& operator()(const int& n1, const int& n2, const int& n3, const int& n4, const int& n5) const
    {
      return t[n1][n2][n3][n4][n5];
    }
  

  const Tensor5<T>& operator=(const Tensor5<T>&);
  bool operator==(const Tensor5<T>&) const;
  bool operator!=(const Tensor5<T>& t1) const
    {
      return !(*this == t1);
    }
  void reset()
  {
    Dim1 = 0;
    Dim2 = 0;
    Dim3 = 0;
    Dim4 = 0;
    Dim5 = 0;
    t.reset();
  }
  
};

template<typename T>
Tensor5<T>::Tensor5(const int& dim1, const int& dim2, const int& dim3, const int& dim4, const int& dim5)
{  
  init(dim1, dim2, dim3, dim4, dim5);
}

template<typename T>
void Tensor5<T>::init(const int& dim1, const int& dim2, const int& dim3, const int& dim4, const int& dim5)
{
  /* error handling */
  if (dim1 <= 0 || dim2 <= 0 || dim3 <= 0 || dim4 <= 0 || dim5 <= 0)
    error("Tensor5::Tensor5", "Cannot initialize with dimension <= 0");
  
  Dim1 = dim1;
  Dim2 = dim2;
  Dim3 = dim3;
  Dim4 = dim4;
  Dim5 = dim5;

  t.init(dim1);
  
  for (int i = 0; i < dim1; i++)
    {
      t[i].init(dim2);
      for (int j = 0; j < dim2; j++)
	{
	  t[i][j].init(dim3);
	  for (int k = 0; k < dim3; k++)
	    {
	      t[i][j][k].init(dim4);
	      for (int l = 0; l < dim4; l++)
		{
		  t[i][j][k][l].init(dim5);
		}
	    } 
	}
    }
}

template<typename T>
const Tensor5<T>& Tensor5<T>::operator=(const Tensor5<T>& t1)
{ 
  if (Dim1 == t1.Dim1 &&
      Dim2 == t1.Dim2 &&
      Dim3 == t1.Dim3 &&
      Dim4 == t1.Dim4 &&
      Dim5 == t1.Dim5)
    {
      
      for (int i = 0; i < Dim1; i++)
	for (int j = 0; j < Dim2; j++)
	  for (int k = 0; k < Dim3; k++)
	    for (int l = 0; l < Dim4; l++)
	      for (int m = 0; m < Dim5; m++)
		{
		  t[i][j][k][l][m] = t1(i, j, k, l, m);
		}
    }
  else
    {
      reset(); 
      init(t1.Dim1, t1.Dim2, t1.Dim3, t1.Dim4, t1.Dim5);
      return (*this = t1);
    }
  
  return *this;
}

template<typename T>
bool Tensor5<T>::operator==(const Tensor5<T>& t1) const
{
  if (Dim1 != t1.Dim1 || Dim2 != t1.Dim2 || Dim3 != t1.Dim3 || Dim4 != t1.Dim4
      || Dim5 != t1.Dim5)
    return false;

  for (int i = 0; i < Dim1; i++)
    for (int j = 0; j < Dim2; j++)
      for (int k = 0; k < Dim3; k++)
	for (int l = 0; l < Dim4; l++)
	  for (int m = 0; m < Dim5; m++)
	    if (t[i][j][k][l][m] != t1(i,j,k,l,m))
	      return false;

  return true;
}
  
#endif
