#include "Failure.h"
#include "PhysicalLayer.h"

#include <iostream>

void PhysicalLayer::computeNextQ(Maneuver sigma, /* For platoon  */ List<State> qList, bool debug)
{
  Control c;
  Failure failure;

  CONF.failures.find(Failure(idx), failure);

  Iterator<Failure::ControlFailure> failItr(failure.ctrlFailures);
  Failure::ControlFailure fail;

  bool foundFail = false;

  while (failItr(fail))
  {
    c = computeControl(sigma, q, /* For platoon */ qList, fail, tooClose, idx, debug);
    foundFail = true;
    break;
  }


  if (!foundFail)
    c = computeControl(sigma, q, /* For platoon */ qList, idx, debug);

  if (CONF.debug)
    LOG.s << "Computing nextQ state..." << EndLine();
  nextQ.theta = q.theta + c.omega;
  nextQ.x = q.x + q.v * cos(q.theta) /* complete form */ +
            q.v * (-1.0 / 6.0 * cos(q.theta) * c.omega * c.omega/*t^3*/ - 0.5 * sin(q.theta) * c.omega/*t^2*/) +
            c.a * (0.5 * cos(q.theta)/*t^2*/ - 0.125 * cos(q.theta) * c.omega * c.omega /*t^4*/ - 1.0 / 3.0 * sin(q.theta) * c.omega /*t^3*/);
  nextQ.y = q.y + q.v * sin(q.theta) /* complete form */ +
            q.v * (0.5 * c.omega * cos(q.theta)/*t^2*/ - 1.0 / 6.0 * sin(q.theta) * c.omega * c.omega/*t^3*/) +
            c.a * (1.0 / 3.0 * cos(q.theta) * c.omega/*t^3*/ + 0.5 * sin(q.theta)/*t^2*/ - 0.125 * sin(q.theta) * c.omega * c.omega/*t^4*/);
  nextQ.v = q.v + c.a;
  //if (nextQ.v > maxV) nextQ.v = maxV;
  if (nextQ.v <= 0) nextQ.v = 0;
  if (CONF.debug)
    LOG.s << "Next state = " << nextQ << EndLine();

}
