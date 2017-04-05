#ifndef STANDARD_RULES_H
#define STANDARD_RULES_H

#include <cmath>

#include "Area.h"
#include "Transition.h"
#include "systemParms.h"
#include "systemTypes.h"
#include "Vector.h"
#include "DynVector.h"

/// Rules parameters:
namespace standardRules
{
  const int N_SUB_EVENT = 32;
  const int N_EVENT = 23;
/* LANES: see also definition.h */

/// ---------------------------------------------------------------------------
/// Rules parameters
/// ---------------------------------------------------------------------------

  double dForward();
  double dBack();
  
  /// ---------------------------------------------------------------------------
  /// SubEvents functions
  /// ---------------------------------------------------------------------------
  
  bool forwardBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool leftBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool rightBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool rightBackBlocking(const Sensing&, const IntVars&, const Sensing&);
  bool forwardPresent(const Sensing&, const IntVars&, const Sensing&);
  bool backPresent(const Sensing&, const IntVars&, const Sensing&);
  bool forwardCompatible(const Sensing&, const IntVars&, const Sensing&);
  bool rightForwardIncompatible(const Sensing&, const IntVars&, const Sensing&);
  bool rightForwardCompatible(const Sensing&, const IntVars&, const Sensing&);
  bool backCompatible(const Sensing&, const IntVars&, const Sensing&);
  bool LeftPlatoonCompatible(const Sensing&, const IntVars&, const Sensing&);
  bool RightPlatoonCompatible(const Sensing&, const IntVars&, const Sensing&);
  bool RightBlockingOvertakeable(const Sensing&, const IntVars&, const Sensing&);
  bool maxLane(const Sensing&, const IntVars&, const Sensing&);
  bool minLane(const Sensing&, const IntVars&, const Sensing&);
  bool targetLane(const Sensing&, const IntVars&, const Sensing&);
  bool linedUp(const Sensing&, const IntVars&, const Sensing&);
  
  /// ---------------------------------------------------------------------------
  /// ``Indicator'' functions
  /// ---------------------------------------------------------------------------
  
  void forwardArea(const Sensing&, Area&);
  void forwardVisibleArea(const Sensing&, Area&);
  void backArea(const Sensing&, Area&);
  void backVisibleArea(const Sensing&, Area&);
  void leftArea(const Sensing&, Area&);
  void leftForwardVisibleArea(const Sensing&, Area&);
  void rightArea(const Sensing&, Area&);
  void rightForwardVisibleArea(const Sensing&, Area&);
  void rightBackArea(const Sensing&, Area&);
  
  /// ---------------------------------------------------------------------------
  /// IntVar reset functions
  /// ---------------------------------------------------------------------------
  
  IntVars setTargetLane(const Sensing&);
  IntVars incTargetLane(const Sensing&);
  IntVars decTargetLane(const Sensing&);
  
  /// ---------------------------------------------------------------------------
  /// Active area
  /// ---------------------------------------------------------------------------
  
  /* return the active area for an agent */
  void activeArea(const Sensing&, Area&);
  
  /// ---------------------------------------------------------------------------
  /// Rules init
  /// ---------------------------------------------------------------------------
  
  void initStandardRules(DynVector<SubEvent>& se, DynVector<Event>& e,
			 Vector<Vector<Transition, N_MANEUVER>, N_MANEUVER>& t,
			 Vector<ResetFunction, N_MANEUVER>& r);
}

#endif
  
