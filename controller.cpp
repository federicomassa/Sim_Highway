
#include "controller.h"
#include "rules.h"
#include <iostream>

Control computeControl(Maneuver sigma, const State& q, /* For platoon */ List<State> qList, int idx, bool debug)
{
	Control c;

	if (CONF.debug)
	{
		LOG.s << "Computing control for q = " << q << ", sigma = " << sigma << ":";
		LOG.s << EndLine(EndLine::INC);
	}

	switch (sigma)
	{
	case PLATOON:
	{
		if (debug)
		{
			std::cout << "See: " << qList.count() << " vehicles" << std::endl;
			const State* q;
			for (int i = 0; i < qList.count(); i++)
			{
				qList.getElem(q, i);
				std::cout << *q << std::endl;
			}
		}

		/* Platoon case FIXME */
		// Look for vehicle ahead and behind
		Iterator<State> is(qList);
		State tmpQ(0, 0, 0, 0);
		State aheadQ(0, 0, 0, 0);
		State behindQ(0, 0, 0, 0);

		bool isVehicleAheadCompatible = false;
		bool isVehicleBehindCompatible = false;
		bool isVehicleAheadBlocking = false;


		for (int i = 0; i < qList.count(); i++) {
			is(tmpQ);

			/* If same lane */
			if (tmpQ.y < floor(q.y) + 1 && tmpQ.y > floor(q.y)) {
				/* If ahead of you */
				if (tmpQ.x - q.x > 0 && fabs(tmpQ.desiredV - q.desiredV) < V_TOLERANCE * q.desiredV) {
					aheadQ = tmpQ;
					isVehicleAheadCompatible = true;
					continue;
				}
				/* If behind you */
				else if (tmpQ.x - q.x < 0 && fabs(tmpQ.desiredV - q.desiredV) < V_TOLERANCE * q.desiredV) {
					behindQ = tmpQ;
					isVehicleBehindCompatible = true;
					continue;
				}

				else if (forwardBlocking(q, floor(q.y), tmpQ)) {
					aheadQ = tmpQ;
					isVehicleAheadBlocking = true;
					continue;
				}



			}
		}



		// if there is both a vehicle ahead and behind
		if (isVehicleAheadCompatible && isVehicleBehindCompatible) {
			c.a = -B_BACK * (q.x - behindQ.x) - B_FORWARD * (q.x - aheadQ.x) - GAMMA * (B_BACK * (q.v - behindQ.v) + B_FORWARD * (q.v - aheadQ.v));
		}

		// Platoon rear
		else if (isVehicleAheadCompatible && !isVehicleBehindCompatible) {
			c.a = -B_FORWARD * (q.x - aheadQ.x + D_REF) - GAMMA * B_FORWARD * (q.v - aheadQ.v);
		}

		// Platoon leader
		else if (!isVehicleAheadCompatible && isVehicleBehindCompatible) {

			// std::cout << "isVehicleAheadBlocking: " << isVehicleAheadBlocking << std::endl;
			// std::cout << "Ahead distance: " << aheadQ.x - q.x << std::endl;
			// std::cout << "Behind distance: " << q.x - behindQ.x << std::endl;
			// std::cout << "My speed: " << q.v << std::endl;
			// std::cout << "Ahead speed: " << aheadQ.v << std::endl;
			// std::cout << "dForward: " << dForward() << std::endl;


			if (isVehicleAheadBlocking) {
				c.a = -B_BACK * (q.x - behindQ.x - D_REF) - B_FORWARD * (q.x - aheadQ.x + dForward()) - GAMMA * (B_BACK * (q.v - behindQ.v) + B_FORWARD * (q.v - aheadQ.v));
			}
			else
			{
				c.a = -B_BACK * (q.x - behindQ.x - D_REF) /*- GAMMA*B_BACK*(q.v - behindQ.v)*/ - 10 * K_LEADER * (q.v - q.desiredV);

			}

			//	    std::cout << "a: " << c.a << std::endl;

		}

		else
		{
			c.a = ACCELERATION;
			if (debug)
				std::cout << "no case" << std::endl;
		}

		/*	  if (fabs(c.a) > ACCELERATION)
		  c.a = fabs(c.a)/c.a*ACCELERATION;*/

		if (q.theta != 0) {
			c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
			          / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
		} else
			c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;

		/*	  if (idx == 2)
		  {
		    std::cout << "aheadComp: " << isVehicleAheadCompatible << std::endl;
		    std::cout << "backComp: " << isVehicleBehindCompatible << std::endl;
		    std::cout << "aheadBlock: " << isVehicleAheadBlocking << std::endl;
		    std::cout << "v: " << q.v << std::endl;
		    }*/


		break;
	}
	case FAST:
	{

		if (q.v < q.desiredV)
			c.a = ACCELERATION;
		else
			c.a = 0;


		if (q.theta != 0) {
			c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
			          / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
		} else
			c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;

		break;
	}
	case SLOW:
		c.a = -ACCELERATION;
		if (q.theta != 0) {
			c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
			          / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
		} else {
			c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
		}
		break;
	case LEFT:
		if (q.v < q.desiredV)
			c.a = ACCELERATION;
		else
			c.a = 0;

		c.omega = MAX_ANGULAR_SPEED;
		break;
	case RIGHT:
		if (q.v < q.desiredV)
			c.a = ACCELERATION;
		else
			c.a = 0;

		c.omega = -MAX_ANGULAR_SPEED;
		break;
	default:
		error("computeControl", "maneuver UNKNOWN passed");
	}


	if (c.omega > 0)
	{
		if (c.omega > MAX_ANGULAR_SPEED)
			c.omega = MAX_ANGULAR_SPEED;
		if (q.theta + c.omega > MAX_THETA)
			//c.omega = 0;
			c.omega = MAX_THETA - q.theta;
	}
	else if (c.omega < 0)
	{
		if (c.omega < -MAX_ANGULAR_SPEED)
			c.omega = -MAX_ANGULAR_SPEED;
		if (q.theta + c.omega < -MAX_THETA)
			//c.omega = 0;
			c.omega = -MAX_THETA - q.theta;
	}

	if (CONF.debug)
		LOG.s << c << EndLine(EndLine::DEC);

	return c;
}

Control computeControl(Maneuver sigma, const State& q, /* For platoon */ List<State> qList, const Failure::ControlFailure& fail, bool& tooClose, int idx, bool debug)
{
	Control c;
	std::cout << "In computeControl fail" << std::endl;
	std::cout << "Too close: " << ((tooClose) ? "True" : "False") << std::endl;
	std::cout << q << std::endl;
	std::cout << qList << std::endl;
	if (fail == Failure::MIMICPLATOON && sigma == PLATOON)
	{
		std::cout << "Found fail" << std::endl;
		// Mimics a forward vehicle, compatible desiredV, that makes it oscillate.
		Iterator<State> is(qList);
		State tmpQ(0, 0, 0, 0);
		State behindQ(0, 0, 0, 0);

		bool isVehicleBehindCompatible = false;


		for (int i = 0; i < qList.count(); i++) {
			is(tmpQ);

			/* If same lane */
			if (tmpQ.y < floor(q.y) + 1 && tmpQ.y > floor(q.y)) {
				/* If behind you */
				if (tmpQ.x - q.x < 0)
				{
					behindQ = tmpQ;
					if (fabs(tmpQ.desiredV - q.desiredV) < V_TOLERANCE * q.desiredV) {
						std::cout << "compatible" << std::endl;
						isVehicleBehindCompatible = true;
						continue;
					}
				}
			}
		}

		// Mimic aheadQ: jamming-like. Vehicle mimicked is
		State aheadQ;

		if (q.x - behindQ.x < (D_REF - 0.3 * D_REF))
		{
			tooClose = true;
		}
		else if (q.x - behindQ.x > (D_REF + 0.3 * D_REF))
		{
			tooClose = false;
		}

		if (tooClose)
			aheadQ.init(q.x + 1.1 * (q.x - behindQ.x), floor(q.y) + 0.5, 0.0, (q.v + 1.1*(q.v - behindQ.v)));
		else
			aheadQ.init(q.x + 0.9 * (q.x - behindQ.x), floor(q.y) + 0.5, 0.0, (q.v + 0.9*(q.v - behindQ.v)));


		if (isVehicleBehindCompatible)
		{
			std::cout << "Comp!" << std::endl;
			c.a = -B_BACK * (q.x - behindQ.x) - B_FORWARD * (q.x - aheadQ.x) - GAMMA * (B_BACK * (q.v - behindQ.v) + B_FORWARD * (q.v - aheadQ.v));
		}
		else
		{
			c.a = -B_FORWARD * (q.x - aheadQ.x + D_REF) - GAMMA * B_FORWARD * (q.v - aheadQ.v);
		}




		if (q.theta != 0)
		{
			c.omega = -(q.y - (floor(q.y) + 0.5)) * q.v * sin(q.theta)
			          / q.theta - LOOP_GAIN_CONSTANT * q.v * q.theta;
		}
		else
		{
			c.omega = - (q.y - (floor(q.y) + 0.5) ) * q.v;
		}
	}

	else return computeControl(sigma, q, qList, idx, debug);


	if (c.omega > 0)
	{
		if (c.omega > MAX_ANGULAR_SPEED)
			c.omega = MAX_ANGULAR_SPEED;
		if (q.theta + c.omega > MAX_THETA)
			//c.omega = 0;
			c.omega = MAX_THETA - q.theta;
	}
	else if (c.omega < 0)
	{
		if (c.omega < -MAX_ANGULAR_SPEED)
			c.omega = -MAX_ANGULAR_SPEED;
		if (q.theta + c.omega < -MAX_THETA)
			//c.omega = 0;
			c.omega = -MAX_THETA - q.theta;

	}

	return c;

}
