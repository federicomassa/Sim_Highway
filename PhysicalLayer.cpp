
#include "PhysicalLayer.h"

void PhysicalLayer::computeNextQ(Maneuver sigma, /* For platoon  */ List<State> qList)
{
  Control c = computeControl(sigma, q, /* For platoon */ qList);

    if(CONF.debug)
        LOG.s << "Computing nextQ state..." << EndLine();
    nextQ.theta = q.theta + c.omega;
    nextQ.x = q.x + q.v * cos(q.theta);
    nextQ.y = q.y + q.v * sin(q.theta);
    nextQ.v = q.v + c.a;
    if (nextQ.v > maxV) nextQ.v = maxV;
    else if (nextQ.v <= 0) nextQ.v = 0;
    if(CONF.debug)
        LOG.s << "Next state = " << nextQ << EndLine();
}
