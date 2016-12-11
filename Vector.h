/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Vector.h
 * DESCRIPTION: Template for the Vector class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef VECTOR_H
#define VECTOR_H

#include "EndLine.h"
#include "utility.h"

/*!
 * This template represents a vector of N elements of type T with a few methods
 * implemented. Note that, in order to use method sort(), operators > and <
 * MUST be redefined for type T.
 */
template<typename T, int N>
class Vector
{
    /*!
     * \brief Redefinition of operator <<.
     */
    friend ostream& operator<<(ostream& os, const Vector<T, N>& v)
    {
        os << "Vector (" << EndLine(EndLine::INC);
        for(int i = 0; i < N; i++)
        {
            os << '[' << i << "]\t=> " << v.elements[i];
            if(i == N - 1)
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
    friend bool operator==(const Vector<T, N>& v1, const Vector<T, N>& v2)
    {
        for(int i = 0; i < N; i++)
            if(v1.elements[i] != v2.elements[i])
                return false;
        
        return true;
    }
    /*!
     * \brief Redefinition of operator !=.
     */
    friend bool operator!=(const Vector<T, N>& v1, const Vector<T, N>& v2)
    {
        return !(v1 == v2);
    }
    /*!
     * \brief Vector of elements T.
     */
    T elements[N];
    /*!
     * \brief Copy function.
     */
    void cp(const T v[]);
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
    /*!
     * \brief Number of elements.
     */
    static const int length = N;
    /*!
     * \brief Default constructor.
     */
    Vector() { }
    /*!
     * \brief Alternative constructor.
     */
    explicit Vector(T val);
    /*!
     * \brief Alternative constructor.
     */
    explicit Vector(const T v[]) { cp(v); }
    /*!
     * \brief Copy constructor.
     */
    Vector(const Vector<T, N>& v) { cp(v.elements); }
    /*!
     * \brief Destructor.
     */
    ~Vector() { }
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
    void sort() { quickSort(0, N - 1); }
    /*!
     * \brief Redefinition of operator =.
     */
    Vector<T, N>& operator=(const Vector<T, N>& v)
    {
        cp(v.elements);
        return *this;
    }
    /*!
     * \brief Redefinition of operator =.
     */
    Vector<T, N>& operator=(const T v[])
    {
        cp(v);
        return *this;
    }
};

template<typename T, int N>
void Vector<T, N>::cp(const T v[])
{
    for(int i = 0; i < N; i++)
        elements[i] = v[i]; /* copy elements */
}

template<typename T, int N>
Vector<T, N>::Vector(T val)
{
    for(int i = 0; i < N; i++)
        elements[i] = val;
}

template<typename T, int N>
const T& Vector<T, N>::operator[](int i) const
{
    /* error handling */
    if(i >= length || i < 0)
        error("Vector", "invalid index reference");
    
    return elements[i];
}

template<typename T, int N>
T& Vector<T, N>::operator[](int i)
{
    /* error handling */
    if(i >= length || i < 0)
        error("Vector", "invalid index reference");
    
    return elements[i];
}

template<typename T, int N>
void Vector<T, N>::quickSort(int inf, int sup)
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
