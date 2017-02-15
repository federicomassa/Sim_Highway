/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Message.h
 * DESCRIPTION: Template for the Message class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include "EndLine.h"
#include "utility.h"

/*!
 * This template represents a message of type T with basic methods implemented.
 * Agents use a message to communicate.
 */
template<typename T>
class Message
{
    /*!
     * \brief Redefinition of operator <<.
     */
    friend std::ostream& operator<<(std::ostream& os, const Message<T>& m)
    {
        os << "Message {" << EndLine(EndLine::INC);
        os << "AgentID: ";
        os.fill('0');
        os.width(2);
        os << m.agentID << EndLine();
        os << "Body: {" << EndLine(EndLine::INC);
        os << m.body << EndLine(EndLine::DEC);
        os << '}' << EndLine(EndLine::DEC);
        os << '}';

        return os;
    }
    /*!
     * \brief Redefinition of operator ==.
     *
     * This is the redefinition of operator ==.
     * Two messages are equals if sender agent identifiers coincide.
     */
    friend bool operator==(const Message<T>& m1, const Message<T>& m2)
    {
        return m1.agentID == m2.agentID;
    }
    /*!
     * \brief Redefinition of operator != using operator ==.
     */
    friend bool operator!=(const Message<T>& m1, const Message<T>& m2)
    {
        return !(m1 == m2);
    }
    /*!
     * \brief Sender agent identifier.
     */
    int agentID;
    /*!
     * \brief Generic message body of type T.
     */
    T body;
    /*!
     * \brief Initialize sender agent identifier.
     *
     * This method initializes the sender agent identifier of a message,
     * which MUST be a non-negative integer.
     * @param a sender agent identifier.
     */
    void initAgentID(int a);
public:
    /*!
     * \brief Default constructor.
     */
    Message() { agentID = -1; }
    /*!
     * \brief Alternative constructor.
     *
     * This is an alternative constructor that receives as input an integer
     * representing the sender agent identifier.
     * @param a sender agent identifier.
     */
    explicit Message(int a) { initAgentID(a); }
    /*!
     * \brief Alternative constructor.
     *
     * This is an alternative constructor that receives as input an integer
     * representing the sender agent identifier and an object of type T
     * representing the message body.
     * @param a sender agent identifier.
     * @param b message body of type T.
     */
    Message(int a, const T& b) { initAgentID(a); body = b; }
    /*!
     * \brief Destructor.
     */
    ~Message() { }
    /*!
     * \brief Get sender agent identifier.
     *
     * @return Sender agent identifier.
     */
    int getAgentID() const { return agentID; }
    /*!
     * \brief Get message body.
     *
     * @param b body read from the message.
     */
    void getBody(T& b) const { b = body; }
    /*!
     * \brief Fill message body.
     *
     * @param b body to insert.
     */
    void putBody(const T& b) { body = b; }
};

template<typename T>
void Message<T>::initAgentID(int a)
{
    /* error handling */
    if(a < 0)
        error("Message", "agentID cannot be negative");

    /* initialization */
    agentID = a;
}

#endif
