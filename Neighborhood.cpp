/*
 * DESIGN:      Cooperative Multi-Agent Systems Simulator
 *
 * FILE:        Neighborhood.cpp
 * DESCRIPTION: Implementation of the Neighborhood class
 *
 * AUTHORS:     Marco Pellinacci <marco.pellinacci@alice.it>
 *              Gianni Valenti <posta@gianni.valenti.name>
 * DATE:        January 1, 2000
 */

#include "Neighborhood.h"
#include "ExtValue.h"
#include <iostream>

using namespace std;


Neighborhood::Neighborhood(int t, const Sensing& sT, const List<Sensing>& sL)
{
  init(t, sT, sL);
}

void Neighborhood::init(int t, const Sensing& sT, const List<Sensing>& sL)
{
    targetID = t;
    sTarget = sT;

    Iterator<Sensing> is(sL);
    Sensing s;
    
    while (is(s))
      {
	sList.insHead(s);
      }
    
}

void Neighborhood::intersectionWith(const Neighborhood& n)
{
    /* error handling */
    if(targetID != n.targetID)
        error("Neighborhood", "cannot intersect with different targetID");
    
    if(this == &n)
        return;

    
}

RepLevel Neighborhood::getTargetReputation() const
{
  return UNSET;
}


ostream& operator<<(ostream& os, const Neighborhood& n)
{
    os << "Neighborhood {" << EndLine(EndLine::INC);
    os << "Target ID: " << n.targetID << EndLine();
    os << "Sensing list: " << n.sList << EndLine();
    os << '}';

    return os;
}
