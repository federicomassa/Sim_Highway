
#ifndef RULEFUNCTIONS_H
#define RULEFUNCTIONS_H

#include <cmath>

#include "Area.h"
#include "systemParms.h"
#include "systemTypes.h"
#include "Vector.h"

class Sensing;

namespace ruleFunctions
{
  /// ---------------------------------------------------------------------------
  /// Rules parameters
  /// ---------------------------------------------------------------------------
  
  double dForward();
  double dBack();
  
  /// ---------------------------------------------------------------------------
  /// SubEvents functions
  /// ---------------------------------------------------------------------------
  
  bool forwardBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool forwardDangerous(const Sensing&, const IntVars&, const Sensing&);
  bool leftBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool rightBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool rightReturn(const Sensing&, const IntVars&, const Sensing&);
  bool minLane(const Sensing&, const IntVars&, const Sensing&);
  bool trueFunction(const Sensing&, const IntVars&, const Sensing&);  
  
  /// ---------------------------------------------------------------------------
  /// Indicator functions
  /// ---------------------------------------------------------------------------
  
  void forwardArea(const Sensing&, Area&);
  void forwardDangerousArea(const Sensing&, Area&);
  void leftArea(const Sensing&, Area&);
  void rightArea(const Sensing&, Area&);
  void rightReturnArea(const Sensing&, Area&);
}
  
#endif
