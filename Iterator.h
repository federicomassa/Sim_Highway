/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Iterator.h
 * DESCRIPTION: Template for the Iterator class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef ITERATOR_H
#define ITERATOR_H

#include <iostream>
/* pre-declaration */
template<typename T> class List;

/*!
 * This template represents an iterator for a list of elements of type T.
 * An iterator allows to browse one after another all elements of the list;
 * in particular, iterator is defined so that, starting from the head of
 * the list, it returns the information contained in an element and stores
 * the pointer to the next element.
 */
template<typename T>
class Iterator
{
    /*!
     * \brief Pointer to the current element.
     */
    typename List<T>::Elem* p;
public:
    /*!
     * \brief Constructor.
     *
     * This is a constructor that receives as input a generic list of type T
     * in order to initialize member p correctly.
     * @param pi List<T> object.
     */
    Iterator(const List<T>& pi) {p = pi.head;}
    /*!
     * \brief Destructor.
     */
    ~Iterator() { }
    /*!
     * \brief Redefinition of operator ().
     *
     * This is the redefinition of operator ().
     * An iteration consists of reading the current information and updating
     * the pointer p.
     * @param pi List object.
     */
    bool operator()(T& i);
    /*!
     * \brief Initialize pointer p.
     *
     * This method receives as input a generic list of type T in order to
     * reinitialize member p correctly.
     * @param i object of type T.
     */
    void initialize(const List<T>& pi) { p = pi.head; }
    /*!
     * \brief Check the end-of-list.
     *
     * @return True if the end-of-list has been reached, false otherwise.
     */
    bool eol() { return p == NULL; }
};

template<typename T>
bool Iterator<T>::operator()(T& i)
{
    if(p == NULL) /* the end of the list */
        return false;
    /* iteration */
    i = p->info;
    p = p->next;
    return true;
}

#endif
