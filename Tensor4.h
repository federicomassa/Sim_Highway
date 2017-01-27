#ifndef TENSOR4_H
#define TENSOR4_H

#include "DynVector.h"
#include <iostream>

template<typename T>
class Tensor4
{
  DynVector<DynVector<DynVector<DynVector<T> > > > t;
  
 public:
  int Dim1;
  int Dim2;
  int Dim3;
  int Dim4;

  // Constructor with tensor dimensions
  Tensor4(const int&,
	  const int&,
	  const int&,
	  const int&);

  // Default constructor --> t has to be initialized
  Tensor4() {Dim1 = 0; Dim2 = 0; Dim3 = 0; Dim4 = 0;};

  // Copy constructor
  Tensor4(const Tensor4& t1) {
    Dim1 = t1.Dim1;
    Dim2 = t1.Dim2;
    Dim3 = t1.Dim3;
    Dim4 = t1.Dim4;
    t = t1;}
  void init(const int&,
	    const int&,
	    const int&,
	    const int&);
  
  
  
  T& operator()(const int& n1, const int& n2, const int& n3, const int& n4)
    {
      return t[n1][n2][n3][n4];
    }

  const T& operator()(const int& n1, const int& n2, const int& n3, const int& n4) const
    {
      return t[n1][n2][n3][n4];
    }
  

  const Tensor4<T>& operator=(const Tensor4<T>&);
  bool operator==(const Tensor4<T>&) const;
  bool operator!=(const Tensor4<T>& t1) const
    {
      return !(*this == t1);
    }
  void reset()
  {
    Dim1 = 0;
    Dim2 = 0;
    Dim3 = 0;
    Dim4 = 0;
    t.reset();
  }
  
};

template<typename T>
Tensor4<T>::Tensor4(const int& dim1, const int& dim2, const int& dim3, const int& dim4)
{
  init(dim1, dim2, dim3, dim4);
}

template<typename T>
void Tensor4<T>::init(const int& dim1, const int& dim2, const int& dim3, const int& dim4)
{
  Dim1 = dim1;
  Dim2 = dim2;
  Dim3 = dim3;
  Dim4 = dim4;

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
	    }
	  
	}
    }
}

template<typename T>
const Tensor4<T>& Tensor4<T>::operator=(const Tensor4<T>& t1)
{
  if (Dim1 == t1.Dim1 &&
      Dim2 == t1.Dim2 &&
      Dim3 == t1.Dim3 &&
      Dim4 == t1.Dim4)
    {
      
      for (int i = 0; i < Dim1; i++)
	for (int j = 0; j < Dim2; j++)
	  for (int k = 0; k < Dim3; k++)
	    for (int l = 0; l < Dim4; l++)
	      t[i][j][k][l] = t1(i,j,k,l);
    }
  else
    {      
      reset(); 
      init(t1.Dim1, t1.Dim2, t1.Dim3, t1.Dim4);
      return (*this = t1);
    }
  
  return *this;
}

template<typename T>
bool Tensor4<T>::operator==(const Tensor4<T>& t1) const
{
  if (Dim1 != t1.Dim1 || Dim2 != t1.Dim2 || Dim3 != t1.Dim3 || Dim4 != t1.Dim4)
    return false;

  for (int i = 0; i < Dim1; i++)
    for (int j = 0; j < Dim2; j++)
      for (int k = 0; k < Dim3; k++)
	for (int l = 0; l < Dim4; l++)
	  if (t[i][j][k][l] != t1(i,j,k,l))
	    return false;

  return true;
}
  
#endif
