/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        DynVector.h
 * DESCRIPTION: Template for the DynVector class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef DYNVECTOR_H
#define DYNVECTOR_H

#include "EndLine.h"
#include "utility.h"

#include <iostream>

/*!
 * This template represents a vector of N (dynamic) elements of type T with a few methods
 * implemented. Note that, in order to use method sort(), operators > and <
 * MUST be redefined for type T.
 */
template<typename T>
class DynVector
{
  
    /*!
     * \brief Redefinition of operator <<.
     */
    friend std::ostream& operator<<(std::ostream& os, const DynVector<T>& v)
    {
        os << "DynVector (" << EndLine(EndLine::INC);
        for(int i = 0; i < v.length; i++)
        {
            os << '[' << i << "]\t=> " << v.elements[i];
            if(i == v.length - 1)
                os << EndLine(EndLine::DEC);
            else
                os << EndLine();
        }
        os << ')';
        
        return os;
    }
    /*!
     * \brief Redefinition of operator ==.
     */
    friend bool operator==(const DynVector<T>& v1, const DynVector<T>& v2)
    {
      if (v1.length != v2.length)
	return false;
      
      for(int i = 0; i < v1.length; i++)
	if(v1.elements[i] != v2.elements[i])
	  return false;
      
      return true;
    }
    /*!
     * \brief Redefinition of operator !=.
     */
    friend bool operator!=(const DynVector<T>& v1, const DynVector<T>& v2)
    {
        return !(v1 == v2);
    }

    /*!
     * \brief DynVector of elements T.
     */
    T* elements;
    /*!
     * \brief Copy function.
     */
    void cp(const DynVector<T>& v);
    /*!
     * \brief Switch function.
     */
    void sw(int x, int y)
    {
        T tmpT = elements[x];
        elements[x] = elements[y];
        elements[y] = tmpT;
    }
    /*!
     * \brief Quick sort function.
     */
    void quickSort(int inf, int sup);
public:
    // number of elements
    int length;
    /*!
     * \brief Default Constructor -> To initialize run init(int).
     */
    DynVector()
      {elements = NULL;
	length = 0;}
    /*!
     * \brief Constructor.
     */
 DynVector(const int& N)
    {
      elements = NULL;
      init(N);
    }
    /*!
     * \brief Default constructor.
     */
 ~DynVector()
   {
     init(0);
   }
        /*!
     * \brief Copy constructor.
     */
 DynVector(const DynVector<T>& v) { cp(v); }
    /*!
     * \brief Initialization.
     */
 void init(const int& N)
 {
   
   if (N < 0)
     error("DynVector", "length should be > 0");
   
   length = N;
   if (elements == NULL && N != 0) elements = new T[N];
   else if (elements != NULL) {
     delete[] elements;
     elements = NULL;
     if (N > 0)
       elements = new T[N];
   }
 }
 /*!
  * \brief Reset function.
  */
 void reset()
 {
   init(0);
 }
 
 /*!
  * \brief Insert function.
  */
 void insert(const T& el)
 {
   /* Create a temporary vector with bigger size */
   DynVector tmpV(length + 1);
   for (int i = 0; i < length; i++)
     tmpV[i] = elements[i];
   
   tmpV[length] = el;
   
   reset();
   cp(tmpV);
   
 }
 
 /*!
  * \brief Redefinition of operator [].
  */
 const T& operator[](int i) const;
 /*!
     * \brief Redefinition of operator [].
     */
    T& operator[](int i);
    /*!
     * \brief Sorting function.
     *
     * This method sorts the elements of the vector using algorithm quicksort.
     * Note that operator < and operator > MUST be defined for type T.
     */
    void sort() { quickSort(0, length - 1); }
    /*!
     * \brief Redefinition of operator =.
     */
    
    DynVector<T>& operator=(const DynVector<T>& v)
    {
      cp(v);
      return *this;
    }
};

template<typename T>
void DynVector<T>::cp(const DynVector<T>& v)
{
  if (elements == NULL)
    {
      elements = new T[length];
      length = v.length;
      for(int i = 0; i < length; i++)
        elements[i] = v[i]; /* copy elements */
    }
  else
    {
      reset();
      cp(v);
    }
}

template<typename T>
const T& DynVector<T>::operator[](int i) const
{
    /* error handling */
    if(i >= length || i < 0)
      error("DynVector::operator[] const", "invalid index reference " + toString(i) + " with length " + toString(length));
    
    return elements[i];
}

template<typename T>
T& DynVector<T>::operator[](int i)
{
    /* error handling */
    if(i >= length || i < 0)
      error("DynVector::operator[]", "invalid index reference " + toString(i) + " with length " + toString(length));
    
    return elements[i];
}

template<typename T>
void DynVector<T>::quickSort(int inf, int sup)
{
    if(inf + 1 == sup)
    {
        if(elements[inf] > elements[sup])
            sw(inf, sup);
    }
    else if(inf + 1 < sup)
    {
        T pivot = elements[(inf + sup) / 2];
        int i = inf;
        int j = sup;
        do
        {
            while(elements[i] < pivot)
                i++;
            while(elements[j] > pivot)
                j--;
            if(i <= j)
            {
                sw(i, j);
                i++;
                j--;
            }
        } while(i <= j);
        quickSort(inf, j);
        quickSort(i, sup);
    }
}

#endif
