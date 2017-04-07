/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Reputation.h
 * DESCRIPTION: Interface for the Reputation class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#ifndef REPUTATION_H
#define REPUTATION_H

#include "State.h"
#include "ExtValue.h"
#include "List.h"
#include "Logger.h"
#include <string>


extern Logger ResultLog;
/*!
 * \brief Reputation level of an agent may be correct, faulty or uncertain.
 */
enum RepLevel { FAULTY, UNCERTAIN, CORRECT, UNSET };

struct RepRecord
{
  int evalTime;
  std::string rule;
  ExtBool result;

  RepRecord()
  {
    evalTime = -1;
    rule = "";
    result = F;
  }

  void init(const int& eT, const std::string& r, const ExtBool& res)
  {
    evalTime = eT;
    rule = r;
    result = res;
  }

  bool operator==(const RepRecord& rec)
  {
    return (evalTime == rec.evalTime &&
	    rule == rec.rule &&
	    result == rec.result);
  }

  bool operator!=(const RepRecord& rec)
  {
    return !((*this) == rec);
  }
  
};

/*!
 * This class represents the reputation of a target-agent.
 * The object is defined by the agent identifier and the state of a
 * target-agent, and by the corresponding reputation level.
 */
class Reputation
{
  List<RepRecord> history;
public:
    /*!
     * \brief Target agent identifier.
     */
    int targetID;
    /*!
     * \brief Target's state.
     */
    State qTarget;
    /*!
     * \brief Target reputation level.
     */
    RepLevel level;
    /*!
     * \brief Default constructor.
     */
    Reputation() { targetID = -1; level = UNSET;}
    /*!
     * \brief Destructor.
     */
    void addRecord(const int&, const std::string&, const ExtBool&);
    ~Reputation() { }
};

/*!
 * \brief Redefinition of operator <<.
 */
std::ostream& operator<<(std::ostream& os, const Reputation& r);
/*!
 * \brief Redefinition of operator ==.
 *
 * This is the redefinition of operator ==.
 * Two reputations are equals if target agent identifiers coincide and
 * if target's states coincide too.
 */
bool operator==(const Reputation& r1, const Reputation& r2);
/*!
 * \brief Redefinition of operator != using operator ==.
 */
bool operator!=(const Reputation& r1, const Reputation& r2);

#endif
