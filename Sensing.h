
#ifndef SENSING_H
#define SENSING_H

#include "EndLine.h"
#include "systemTypes.h"

class State;

class Sensing
{
    friend bool operator==(const Sensing& s1, const Sensing& s2);
    friend bool operator!=(const Sensing& s1, const Sensing& s2);
    friend bool operator<(const Sensing& s1, const Sensing& s2);
public:
    int agentID;
    double x, y, theta, v;
    double desiredV;
    std::string vehicleType;
    
    Sensing() { agentID = -1; }
    Sensing(int tIdx, const State& tQ, const Parms& tP)
    {
      /* this is the list of available sensor data */
        agentID = tIdx;
        x = tQ.x;
	y = tQ.y;
	theta = tQ.theta;
	v = tQ.v;
        desiredV = tP.desiredV;
	vehicleType = tP.vehicleType;
    }

    Sensing(const int& tIdx, const double& x, const double& y, const double& theta, const double& v,
	    const double& desiredV, const std::string& vehicleType)
      {
	/* this is the list of available sensor data */
        agentID           = tIdx;
        this->x           = x;
	this->y           = y;
	this->theta       = theta;
	this->v           = v;
	this->desiredV    = desiredV;
	this->vehicleType = vehicleType;
    }
    
    Sensing(const Sensing& s)
      {
	*this = s;
      }
    
    const Sensing& operator=(const Sensing& s)
      {
	agentID = s.agentID;
	x = s.x;
	y = s.y;
	theta = s.theta;
	v = s.v;
	desiredV = s.desiredV;
	vehicleType = s.vehicleType;

	return *this;
      }
    
};

std::ostream& operator<<(std::ostream& os, const Sensing& s);

#endif
