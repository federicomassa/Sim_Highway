
#ifndef RULEFUNCTIONS_H
#define RULEFUNCTIONS_H

#include <cmath>

#include "Area.h"
#include "systemParms.h"
#include "systemTypes.h"
#include "Vector.h"

class Sensing;

// Misc:
#define PI 3.14159265358979323846
#define MIN_LANE 0
#define MAX_LANE 1
#define LANE_HEIGHT 1.0
#define DELTA_Y LANE_HEIGHT / 2.0


typedef Vector<Vector<double, 2>, 2> Matrix_2x2;

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
  bool minLane(const Sensing&, const IntVars&, const Sensing&);
  bool trueFunction(const Sensing&, const IntVars&, const Sensing&);  
  
  /// ---------------------------------------------------------------------------
  /// Indicator functions
  /// ---------------------------------------------------------------------------
  
  void forwardArea(const Sensing&, Area&);
  void forwardDangerousArea(const Sensing&, Area&);
  void leftArea(const Sensing&, Area&);
  void rightArea(const Sensing&, Area&);
}
  
#endif
