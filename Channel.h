/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Channel.h
 * DESCRIPTION: Template for the Channel class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include <cmath>
#include <cstdlib>

#include "List.h"
#include "Message.h"
#include "State.h"

/*!
 * This template represents a wireless channel where it is possible to
 * broadcast messages of type T; basic methods are implemented.
 */
template<typename T>
class Channel
{
    /*!
     * \brief Redefinition of operator <<.
     */
    friend std::ostream& operator<<(std::ostream& os, const Channel<T>& c)
    {
        os << "Channel {" << EndLine(EndLine::INC);
        os << c.pktList << EndLine(EndLine::DEC);
        os << '}';

        return os;
    }
    /*!
     * This template represents a packet of type U, which is a data
     * structure only used by the Channel class.
     */
    template<typename U>
    class Packet
    {
        /*!
         * \brief Redefinition of operator <<.
         */
        friend std::ostream& operator<<(std::ostream& os, const Packet<U>& p)
        {
            os << "Packet {" << EndLine(EndLine::INC);
            os << "qSender: " << p.qSender << EndLine();
            os << "Message: " << p.msg << EndLine(EndLine::DEC);
            os << '}';

            return os;
        }
        /*!
         * \brief Redefinition of operator ==.
         *
         * This is the redefinition of operator ==.
         * Two packets are equals if sender's states coincide.
         */
        friend bool operator==(const Packet<U>& p1, const Packet<U>& p2)
        {
            return p1.qSender == p2.qSender;
        }
        /*!
         * \brief Redefinition of operator != using operator ==.
         */
        friend bool operator!=(const Packet<U>& p1, const Packet<U>& p2)
        {
            return !(p1 == p2);
        }
    public:
        /*!
         * \brief Sender's state.
         */
        State qSender;
        /*!
         * \brief Generic message of type U.
         */
        Message<U> msg;
        /*!
         * \brief Default constructor.
         */
        Packet() { }
        /*!
         * \brief Alternative constructor.
         *
         * This is an alternative constructor that receives as input an
         * integer representing the sender agent identifier, a State
         * object representing the sender's state and an object of type
         * U representing the message body.
         * @param a sender agent identifier.
         * @param q sender's state.
         * @param b message body.
         */
        Packet(int a, const State& q, const T& b) : msg(a, b) { qSender = q; }
        /*!
         * \brief Destructor.
         */
        ~Packet() { }
    };
    /*!
     * \brief List of generic packets of type T.
     */
    List<Packet<T> > pktList;
    /*!
     * \brief Communication range.
     */
    double radius;
    /*!
     * \brief A message is received with some probability.
     */
    double prob;
    /*!
     * \brief Check if the receiver is in the sender's communication range.
     *
     * @param qReceiver receiver's state.
     * @param qSender sender's state
     * @return True if the receiver is in the sender's communication range,
     * otherwise false.
     */
    bool inRange(const State& qReceiver, const State& qSender) const
    {
        return qReceiver.distanceFrom(qSender) <= radius;
    }
    /*!
     * \brief Flush the channel.
     */
    void flush() { pktList.purge(); };
public:
    /*!
     * \brief Constructor.
     *
     * This is a constructor that receives as input a double representing
     * the communication range of an agent -- which MUST be non-negative --
     * and a double representing the probability that a message is received
     * -- which MUST be in [0,1].
     * @param r communication range.
     * @param p probability.
     */
    Channel(double r, double p);
    Channel(const Channel<T>&);
    const Channel<T>& operator=(const Channel<T>&);
    /*!
     * \brief Destructor.
     */
    ~Channel() { }
    /*!
     * \brief Broadcast a message.
     *
     * @param a sender agent identifier.
     * @param q sender's state.
     * @param b message body.
     */
    void sendBroadcast(int a, const State& q, const T& b)
    {
        Packet<T> p(a, q, b);
        pktList.insHead(p);
    }
    /*!
     * \brief Receive all appropriate broadcast messages.
     *
     * This method allows an agent in some state to receive all appropriate
     * broadcast messages.
     * @param a receiver agent identifier.
     * @param qReceiver receiver's state.
     * @param msgList received messages.
     */
    void recvBroadcast(int a, const State& qReceiver,
                       List<Message<T> >& msgList);
    /*!
     * \brief Empty the channel.
     */
    void clearAll() { flush(); }
};

template<typename T>
Channel<T>::Channel(double r, double p)
{
    /* error handling */
    if(r < 0 || p < 0 || p > 1)
        error("Channel", "see radius and/or probability");

    /* initialization */
    radius = r;
    prob = p;
}

template<typename T>
Channel<T>::Channel(const Channel<T>& c)
{
  (*this) = c;
}

template<typename T>
const Channel<T>& Channel<T>::operator=(const Channel<T>& c)
{  
  /* initialization */
  radius = c.radius;
  prob = c.prob;
  return (*this);
}


template<typename T>
void Channel<T>::recvBroadcast(int a, const State& qReceiver,
                               List<Message<T> >& msgList)
{
    /* error handling */
    if(msgList.count() > 0)
        error("Channel", "msgList MUST be empty");
    
    /* message list filling */
    Iterator<Packet<T> > pi(pktList);
    Packet<T> tmpPkt;
    while(pi(tmpPkt))
    {
        if(tmpPkt.msg.getAgentID() == a || !inRange(qReceiver, tmpPkt.qSender))
            continue; /* unusable message */
        
        /* now probability comes into play */
        double p = (double)rand() / (double)RAND_MAX; /* p is in [0,1] */

        if(p <= prob) /* message received */
            msgList.insHead(tmpPkt.msg);
    }
}

#endif
