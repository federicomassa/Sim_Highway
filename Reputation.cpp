/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Reputation.cpp
 * DESCRIPTION: Implementation of the Reputation class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Reputation.h"

using namespace std;

ostream& operator<<(ostream& os, const Reputation& r)
{
    os << "Reputation {" << EndLine(EndLine::INC);
    os << "TargetID: " << r.targetID << EndLine();
    os << "Target's sensing: " << r.sTarget << EndLine();
    os << "Reputation level: ";
    switch(r.level)
    {
        case CORRECT:
            os << "CORRECT";
            break;
        case FAULTY:
            os << "FAULTY";
            break;
        case UNCERTAIN:
            os << "UNCERTAIN";
            break;
        case UNSET:
            os << "UNSET";
            break;
    }
    os << EndLine();
    os << '}' << EndLine(EndLine::DEC);

    return os;
}

bool operator==(const Reputation& r1, const Reputation& r2)
{
    return r1.targetID == r2.targetID && r1.sTarget == r2.sTarget;
}

bool operator!=(const Reputation& r1, const Reputation& r2)
{
    return !(r1 == r2);
}

void Reputation::addRecord(const int& eT, const std::string& r, const ExtBool& res, const List<Area>& pArea, const Area& nArea)
{
  RepRecord record;
  record.init(eT, r, res, pArea, nArea);
  history.insHead(record);

  switch (res)
    {
    case T:
      level = FAULTY;
      break;
    case U:
      if (level == CORRECT || level == UNSET)
	level = UNCERTAIN;
      break;
    case F:
      if (level == UNSET)
	level = CORRECT;
      break;
    }

  ResultLog.s << "Evaluating rule: " << r << EndLine(ResultLog.incrementIndentation());
  ResultLog.s << "Result: " << res << EndLine(ResultLog.decrementIndentation());

  
}

void Reputation::reEvaluateLevel()
{
  Iterator<RepRecord> recordItr(history);
  RepRecord record;

  ExtBool levelBool = T;
  
  while (recordItr(record))
    {
      if (record.evalTime < now)
	break;

      levelBool = levelBool && record.result;
    }

  switch (levelBool)
    {
    case F:
      level = CORRECT;
      break;
    case U:
      level = UNCERTAIN;
      break;
    case T:
      level = FAULTY;
      break;
    }
}
