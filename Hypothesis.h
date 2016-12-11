/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Hypothesis.h
 * DESCRIPTION: Interface of the Hypothesis class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef HYPOTHESIS_H
#define HYPOTHESIS_H

#include "Area.h"

/*!
 * This class represents an explanation for the behavior of an agent and
 * contains some details about the events and sub-events that could have
 * provoked that behavior.
 * @see Event
 * @see SubEvent
 */
class Hypothesis
{
public:
    /*!
     * This class contains hypothesis about the activation of a particular
     * subEvent by the value of the continuous state of an agent.
     * @see SubEvent
     */
    class SubHypothesis
    {
        /*!
         * \brief Redefinition of operator <<.
         */
        friend ostream& operator<<(ostream& os, const SubHypothesis& sh);
        /*!
         * \brief Redefinition of operator ==.
         */
        friend bool operator==(const SubHypothesis& se1,
                               const SubHypothesis& se2)
        {
            return se1.subEventID == se2.subEventID;
        }
        /*!
         * \brief Redefinition of operator !=.
         */
        friend bool operator!=(const SubHypothesis& se1,
                               const SubHypothesis& se2)
        {
            return se1.subEventID != se2.subEventID;
        }
        /*!
         * \brief This function is a kind of redefinition of operator sizeof().
         */
        friend int size(SubHypothesis se)
        {
            return size(se.subEventID) + size(se.positive);
        }
    public:
        /*!
         * \brief SubEvent universal index.
         */
        int subEventID;
        /*!
         * \brief Hipotesys about the existance of a vehicle having its
         * continuous state q in this area.
         */
        Area positive;
        /*!
         * \brief Default constructor.
         */
        SubHypothesis() { subEventID = -1; }
        /*!
         * \brief Destructor.
         */
        ~SubHypothesis() { }
        /*!
         * \brief Return a measure of the area -- if any.
         */
        double measure() const { return positive.measure(); }
    };
    /*!
     * \brief Event universal index.
     */
    int eventID;
    /*!
     * \brief List of SubHypothesiss.
     */
    List<SubHypothesis> subHypList;
    /*!
     * \brief Hipotesys about the non-existance of a vehicle having its
     * continuous state q in this area.
     */
    Area negative;
    /*!
     * \brief Default constructor.
     */
    Hypothesis() { eventID = -1; }
    /*!
     * \brief Destructor.
     */
    ~Hypothesis() { }
    /*!
     * \brief Return a measure of the uncertainty.
     */
    double measure() const;
};

/*!
 * \brief Redefinition of operator <<.
 */
ostream& operator<<(ostream& os, const Hypothesis& h);
/*!
 * \brief Redefinition of operator <.
 */
inline bool operator<(const Hypothesis& h1, const Hypothesis& h2)
{
    return h1.subHypList.count() < h2.subHypList.count();
}
/*!
 * \brief Redefinition of operator >.
 */
inline bool operator>(const Hypothesis& h1, const Hypothesis& h2)
{
    return h1.subHypList.count() > h2.subHypList.count();
}
/*!
 * \brief Redefinition of operator ==.
 */
inline bool operator==(const Hypothesis& h1, const Hypothesis& h2)
{
    return h1.eventID == h2.eventID;
}
/*!
 * \brief Redefinition of operator !=.
 */
inline bool operator!=(const Hypothesis& h1, const Hypothesis& h2)
{
    return !(h1 == h2);
}
/*!
 * \brief This function is a kind of redefinition of operator sizeof().
 */
inline int size(Hypothesis h)
{
    return size(h.eventID) + size(h.subHypList) + size(h.negative);
}

#endif
