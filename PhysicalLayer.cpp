#include "controller.h"
#include "PhysicalLayer.h"

#include <iostream>

void PhysicalLayer::init(const std::pair<State, Parms>& qp)
{
  
  q = nextQ = qp.first;
  p = qp.second;
  //q.v = 0;
  //nextQ.v = 0;

  /* parse physicalLayer type */
  if (p.pLayerType == "StandardUnicycle")
    controlFuncPointer = &(unicycleControl);
  else
    error("PhysicalLayer::init", "pLayerType " + p.pLayerType + " is unknown");
}

void PhysicalLayer::computeNextQ(const Maneuver& sigma, const List<Sensing>& sList, const bool& debug)
{
  if (controlFuncPointer == 0)
    error("PhysicalLayer::computeNextQ", "Control function was not initialized, please call PhysicalLayer::init first");

  (*controlFuncPointer)(q, nextQ, idx, p, sigma, sList, debug);
}

void unicycleControl(const State& q, State& nextQ, const int& idx, const Parms& p, const Maneuver& sigma, const List<Sensing>& sList, const bool& debug)
{
  Control c = computeControl(sigma, q, p, sList, idx, debug);
  
  if(CONF.debug)
    LOG.s << "Computing nextQ state..." << EndLine();
  
  nextQ.theta = q.theta + c.omega;
  nextQ.x = q.x + q.v * cos(q.theta) /* complete form */ +
    q.v*(-1.0/6.0*cos(q.theta)*c.omega*c.omega/*t^3*/ - 0.5*sin(q.theta)*c.omega/*t^2*/) +
    c.a*(0.5*cos(q.theta)/*t^2*/ - 0.125*cos(q.theta)*c.omega*c.omega /*t^4*/ - 1.0/3.0*sin(q.theta)*c.omega /*t^3*/);
  nextQ.y = q.y + q.v * sin(q.theta) /* complete form */ +
    q.v*(0.5*c.omega*cos(q.theta)/*t^2*/ - 1.0/6.0*sin(q.theta)*c.omega*c.omega/*t^3*/) +
    c.a*(1.0/3.0*cos(q.theta)*c.omega/*t^3*/ + 0.5*sin(q.theta)/*t^2*/ - 0.125*sin(q.theta)*c.omega*c.omega/*t^4*/);
  nextQ.v = q.v + c.a;
  
  if (nextQ.v > MAX_SPEED) nextQ.v = MAX_SPEED;
  else if (nextQ.v <= 0) nextQ.v = 0;
  if(CONF.debug)
    LOG.s << "Next state = " << nextQ << EndLine();
  
}
