/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        List.h
 * DESCRIPTION: Template for the List class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef LIST_H
#define LIST_H

#include "EndLine.h"
#include "Iterator.h"
#include <iostream>
/*!
 * This template represents a list of elements of type T with several typical
 * methods implemented. Note that, in order to use this template, operators ==
 * and != MUST be redefined for type T.
 */
template<typename T>
class List
{
    /*!
     * \brief Class Iterator must be friend of this one.
     */
    friend class Iterator<T>;
    /*!
     * \brief Redefinition of operator <<.
     */
    friend std::ostream& operator<<(std::ostream& os, const List<T>& il)
    {
        Elem* aux = il.head;
        
        /* check if the list is empty */
        if(il.head == NULL)
        {
            os << "List { EMPTY }";
            return os;
        }

        os << "List {" << EndLine(EndLine::INC);
        while(aux)
        {
            os << aux->info;
            aux = aux->next;
            if(aux)
                os << EndLine();
            else
                os << EndLine(EndLine::DEC);
        }
        os << '}';

        return os;
    }
    /*!
     * \brief Redefinition of operator ==.
     *
     * This is the redefinition of operator ==.
     * Two lists are equals if all their corresponding elements coincide
     * and they are the same size.
     */
    friend bool operator==(const List<T>& l1, const List<T>& l2)
    {
        Elem* p = l1.head;
        Elem* q = l2.head;
        while(p != NULL && q != NULL)
        {
            if(p->info != q->info)
                return false;
            
            p = p->next;
            q = q->next;
        }
        
        if(p != NULL || q != NULL)
            return false;
        
        return true;
    }
    /*!
     * \brief Redefinition of operator != using operator ==.
     */
    friend bool operator!=(const List<T>& l1, const List<T>& l2)
    {
        return !(l1 == l2);
    }
    /*!
     * \brief This function is a kind of redefinition of operator sizeof().
     */
    friend int size(List<T> l)
    {
        /* initialization takes the head pointer into account */
        int countSize = 4;
        T i;
        while(l.extrHead(i))
             /* + 4 bytes in order to consider the next pointer too */
            countSize = countSize + size(i) + 4;
        return countSize;
    }
    /*!
     * \brief An element of the list.
     */
    struct Elem
    {
        /*!
         * \brief Generic information of type T.
         */
        T info;
        /*!
         * \brief Pointer to the next element.
         */
        Elem* next;
    };
    /*!
     * \brief Pointer to the head of the list.
     */
    Elem* head;
    /*!
     * \brief Look for a specific information.
     *
     * @param i information to look for.
     * @return A pointer to the first element containing information i
     * or a NULL pointer if there isn't information i.
     */
    Elem* lookFor(const T& i) const;
    /*!
     * \brief Copy the whole list.
     *
     * @param d list to copy from.
     */
    void cp(const List<T>& d);
    /*!
     * \brief Delete the whole list.
     */
    void del();
public:
    /*!
     * \brief Default constructor.
     */
    List() { head = NULL; }
    /*!
     * \brief Copy constructor.
     */
    List(const List<T>& d) { cp(d); }
    /*!
     * \brief Redefinition of operator =.
     */
    List<T>& operator=(const List<T>& d);
    /*!
     * \brief Destructor.
     */
    ~List() { del(); }
    /*!
     * \brief Check if a specific information belongs to the list.
     *
     * @param i information to look for.
     * @return True if information i belongs to the list, otherwise false.
     */
    bool belongs(const T& i) const
    {
        return (lookFor(i) == NULL) ? false : true;
    }
    /*!
     * \brief Check if a specific information belongs to the list and return it.
     *
     * @param i information to look for.
     * @param c desired information -- filled by this method.
     * @return True if information i belongs to the list, otherwise false.
     */
    bool find(const T& i, T& c) const;
    /*!
     * \brief Insert a new element at the top of the list.
     *
     * This method inserts a new element at the top of the list, checking
     * -- if requested -- that such element isn't already contained in
     * the list.
     * @param i information to insert into the list.
     * @param unique indicates if the method must check for uniqueness
     * of information.
     * @return True if insertion happens, otherwise false.
     */
    bool insHead(const T& i, bool unique = false);
    /*!
     * \brief Extract an element from the top of the list and return it.
     *
     * @param i information extracted from the list.
     * @return True if extraction happens, otherwise false -- in this case
     * information i isn't filled.
     */
    bool extrHead(T& i);
    /*!
     * \brief Extract an element from the bottom of the list and return it.
     *
     * @param i information extracted from the list.
     * @return True if extraction happens, otherwise false -- in this case
     * information i isn't filled.
     */
    bool extrTail(T& i);

    /*!
     * \brief Get the first element of the list and return it.
     *
     * @param i information read from the list.
     * @return True if reading happens, otherwise false -- in this case
     * information i isn't filled.
     */
    bool getFirst(T& i) const;
    /*!
     * \brief Insert a new element at the bottom of the list.
     *
     * This method inserts a new element at the bottom of the list,
     * checking -- if requested -- that such element isn't already
     * contained in the list.
     * @param i information to insert into the list.
     * @param unique indicates if the method must check for uniqueness
     * of information.
     * @return True if insertion happens, otherwise false.
     */
    bool insTail(const T& i, bool unique = false);
    /*!
     * \brief Append another list at the bottom of this one.
     *
     * @param l list to append.
     */
    void append(const List<T>& l);
    /*!
     * \brief Join another list to this one without duplicates.
     *
     * @param l list to join.
     * @return True if at least one element has been inserted, otherwise
     * false.
     */
    bool join(const List<T>& l);
    /*!
     * \brief Delete a specific information.
     *
     * @param i information to delete.
     * @return True if deletion is possible, otherwise false.
     */
    bool delInfo(const T& i);
    /*!
     * \brief Delete the whole list.
     */
    void purge() { del(); }
    /*!
     * \brief Update a specific information.
     *
     * @param oldInfo information to update.
     * @param newInfo updated information.
     * @return True if the updating of information has been possible,
     * otherwise false.
     */
    bool updateInfo(const T& oldInfo, const T& newInfo);
    /*!
     * \brief Count the number of elements.
     *
     * @return Number of elements.
     */
    int count() const;
    /*!
     * \brief Check if the list is empty.
     *
     * @return True if the list is empty, otherwise false.
     */
    bool isEmpty() const { return head == NULL; }
    /*!
     * \brief Sorting function.
     *
     * This method sorts the elements of the List.
     * Note that operator > and operator < MUST be defined for type T.
     */
    void sort();

    /*!
     * \brief Get element function.
     *
     * Get a pointer to a specific element of the list
     */
    T* getHead();
    T* getTail();
    void getElem(const T*&, const int&) const;
    void getElem(T*&, const int&) const;
    
};

template<typename T>
typename List<T>::Elem* List<T>::lookFor(const T& i) const
{
    Elem* aux;

    /* search */
    for(aux = head; aux != NULL && aux->info != i; aux = aux->next);

    return aux;
}

template<typename T>
void List<T>::cp(const List<T>& d)
{
    head = NULL;

    /* copy */
    if(d.head != NULL)
    {
        head = new Elem;
        head->info = d.head->info;
        Elem* aux = d.head->next;
        Elem* tmp = head;
        while(aux != NULL)
        {
            tmp->next = new Elem;
            tmp = tmp->next;
            tmp->info = aux->info;
            aux = aux->next;
        }
        tmp->next = NULL;
    }
}

template<typename T>
void List<T>::del()
{
    Elem* aux = head;

    /* deletion */
    while(head != NULL)
    {
        head = head->next;
        delete aux;
        aux = head;
    }
}

template<typename T>
bool List<T>::find(const T& i, T& c) const
{
    Elem* p = lookFor(i);
    
    /* i not found */
    if(p == NULL)
        return false;
    
    /* copy */
    c = p->info;
    return true;
}

template<typename T>
void List<T>::sort()
{
    if(count() > 1)
    {
        bool finished = false;
        while(!finished)
        {
            finished = true;
            Elem* aux1 = head;
            Elem* aux2 = head->next;
            while(aux2)
            {
                if(aux2->info < aux1->info)
                {
                    T tmpInfo = aux1->info;
                    aux1->info = aux2->info;
                    aux2->info = tmpInfo;
                    finished = false;
                }
                aux1 = aux1->next;
                aux2 = aux2->next;
            }
        }
    }
}

template<typename T>
List<T>& List<T>::operator=(const List<T>& d)
{
  
    if(this != &d)
    {
        del();
        cp(d);
    }
    
    return *this;
}

template<typename T>
bool List<T>::insHead(const T& i, bool unique)
{

    /* check if i is contained in the list */
    if (unique && belongs(i))
        return false;

    Elem* newE = new Elem;

    /* insertion at the top of list */
    newE->info = i;
    newE->next = head;
    head = newE;
    
    return true;
}

template<typename T>
bool List<T>::extrHead(T& i)
{   
    if(head == NULL)
        return false;
    
    /* extraction from the top of the list */
    Elem* tmpE = head->next;
    i = head->info;
    delete head;
    head = tmpE;
    
    return true;
}

template<typename T>
bool List<T>::extrTail(T& i)
{   
    if(head == NULL)
        return false;

    
    int length = count();

    //if only one element, head must be moved
    if (length == 1)
      return extrHead(i);
    
    /* extraction from the bottom of the list */
    Elem* penultimate = head;

    //take penultimate element
    for (int n = 0; n < length - 2; n++)
      penultimate = penultimate->next;

    //now take last element 
    Elem* last = penultimate->next;

    //penultimate will become last
    penultimate->next = NULL;

    
    i = last->info;
    delete last;
        
    return true;
}

template<typename T>
T* List<T>::getTail()
{
    if (head == NULL)
        return NULL;

    int length = count();

    Elem* elem = head;
    for (int i = 0; i < length - 1; i++)
    {
        elem = elem->next;
    }

    return elem;
}

template<typename T>
T* List<T>::getHead()
{
    return head;
}

template<typename T>
bool List<T>::getFirst(T& i) const
{   
    if(head == NULL)
        return false;
    
    /* retur/n the top of the list */
    i = head->info;
    
    return true;
}

template<typename T>
bool List<T>::insTail(const T& i, bool unique)
{
    /* check if i is contained in the list */
  if (unique && belongs(i))
    return false;
    
    Elem* newE = new Elem;

    /* insertion at the bottom of list */
    newE->info = i;
    newE->next = NULL;
    
    if(head == NULL)
        head = newE;
    else
    {
        Elem* aux;
        for(aux = head; aux->next != NULL; aux = aux->next);
        aux->next = newE;
    }
    
    return true;
}

template<typename T>
void List<T>::append(const List<T>& l)
{
    /* copy the list to append */
    List<T> tmpList = l;

    /* appending */
    if(head == NULL)
        head = tmpList.head;
    else
    {
        Elem* aux;
        for(aux = head; aux->next != NULL; aux = aux->next);
        aux->next = tmpList.head;
    }

    /* deleting all references to tmpList's items */
    tmpList.head = NULL;
}

template<typename T>
bool List<T>::join(const List<T>& l)
{
    /* chek if the list l is empty */
    if(l.head == NULL)
        return true;

    /* join */
    bool result = false;
    Elem* aux;
    for(aux = l.head; aux != NULL; aux = aux->next)
    {
        bool tmp = insHead(aux->info, true);
        result = result || tmp;
    }
    
    return result;
}

template<typename T>
bool List<T>::delInfo(const T& i)
{
    Elem* p = NULL;
    Elem* q;

    for(q = head; q != NULL && q->info != i; q = q->next)
        p = q;

    if(q == NULL)
        return false;

    /* deletion */
    if(q == head)
        head = q->next;
    else
        p->next = q->next;
    delete q;

    return true;
}

template<typename T>
bool List<T>::updateInfo(const T& oldInfo, const T& newInfo)
{
    Elem* aux = lookFor(oldInfo);

    if(aux == NULL) /* info doesn't belong to the list */
        return false;
    /* info belongs to the list */
    aux->info = newInfo; /* updating */

    return true;
}

template<typename T>
int List<T>::count() const
{
    int n = 0;
    Elem* aux;

    for(aux = head; aux != NULL; aux = aux->next)
        n++;

    return n;
}

template<typename T>
void List<T>::getElem(const T*& elem, const int& index) const
{
  /* error handling */
  if (index >= count())
    {
      elem = NULL;
      return;
    }
  
  Elem* aux = head;
  
  for (int i = 0; i < index; i++)
    {
      aux = aux->next;
    }

  elem = &(aux->info);
}

template<typename T>
void List<T>::getElem(T*& elem, const int& index) const
{
  /* error handling */
  if (index >= count())
    {
      elem = NULL;
      return;
    }
  
  Elem* aux = head;
  
  for (int i = 0; i < index; i++)
    {
      aux = aux->next;
    }

  elem = &(aux->info);
}

#endif
