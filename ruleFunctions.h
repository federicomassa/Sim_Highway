
#ifndef RULEFUNCTIONS_H
#define RULEFUNCTIONS_H

#include <cmath>

#include "Area.h"
#include "systemParms.h"
#include "systemTypes.h"
#include "Vector.h"

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
  
  bool forwardBlocking(const State&, const IntVars&, const State&);
  bool forwardDangerous(const State&, const IntVars&, const State&);
  bool leftBlocking(const State&, const IntVars&, const State&);
  bool rightBlocking(const State&, const IntVars&, const State&);
  bool minLane(const State&, const IntVars&, const State&);  
  
  /// ---------------------------------------------------------------------------
  /// Indicator functions
  /// ---------------------------------------------------------------------------
  
  void forwardArea(const State&, Area&);
  void forwardDangerousArea(const State&, Area&);
  void leftArea(const State&, Area&);
  void rightArea(const State&, Area&);
}
  
#endif
