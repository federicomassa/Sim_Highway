#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <iomanip>
#include <cmath>

#include "systemParms.h"
#include "systemTypes.h"
#include "utility.h"
#include "Vector.h"

class State
{
	friend int size(State s)
	{
		return size(s.x) + size(s.y) + size(s.theta) + size(s.v);
	}



	/* initialize state's members */
public:
	double x;
	double y;
	double theta;
	double v;

	//Mainly for platoon --- Added by Federico Massa
	double desiredV;
	std::string initManeuver;

	/* constructor */
	State() {};
	State(double qX, double qY, double qTheta, double qV, double qDesiredV = 0.0, std::string qInitManeuver = "FAST");
	State(const std::string& str);
	void init(double qX, double qY, double qTheta, double qV, double qDesiredV = 0.0, std::string qInitManeuver = "FAST");
	State(const State&);
	/* destructor */
	~State() { }
	/* calculate the distance from another state */
	double distanceFrom(const State& s) const
	{
		return sqrt(pow(x - s.x, 2) + pow(y - s.y, 2));
	}
	/* conversion from State to double[Q_DIM] */
	Vector<double, 2> toPoint() const;

	double& operator[](const int&);
};

std::ostream& operator<<(std::ostream&, const State&);
bool operator==(const State&, const State&);
bool operator!=(const State&, const State&);
State operator+(const State&, const State&);
State operator-(const State&, const State&);
State operator/(const State&, const double&);


#endif
