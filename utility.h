/**
 * DESIGN:      Reputation/Trust System in Cooperative Multi-Agent Systems
 *
 * FILE:        Channel.h
 * DESCRIPTION: interface for the Channel class
 *
 * AUTHOR:      Marco Pellinacci
 * DATE:        June 9, 2007
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <new>
#include <string>
#include <sstream>
#include <vector>
#include "systemParms.h"

class Sensing;
template<class T, int N> class Vector;

/**
 * this function gives information about a specific error and it terminates
 * the program
 */
void error(const std::string className, const std::string msg, bool exc = false);

/**
 * this function is called by set_new_handler()
 */
void exaustedMem();

/**
 * this function returns the sign (+1 or -1) of a double
 */
int sign(const double);

std::string toString(int i, int pad = 0);

std::string toStringWithPrecision(double i, int precision = 0);

double setPrecision(const double& x, const int& precision);
double floorPrecision(const double& x, const int& precision);
double ceilPrecision(const double& x, const int& precision);

inline int size(int i) { return sizeof(i); }

inline int size(double d) { return sizeof(d); }

inline int size(std::string s) {return sizeof(s);}

void circle3points(const double&, const double&,
		   const double&, const double&,
		   const double&, const double&,
		   double&, double&, double&);

/* split string into elements separated by a delimiter */
std::vector<std::string> split(const std::string&, const std::string&);

/* least-square fit of a circle. 
 Measurements are taken from the last n elements of the vector states. 
 Results of the fit are xc, yc, R of the circumference.
*/
void circleFit(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, double& xc, double& yc, double& R);
/* compute minimum distance of point (x0, y0) from circle with center xc, yc and radius r */
double distanceFromCircle(const double& x0, const double& y0, const double& xc, const double& yc, const double& r);
double fitDistance(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, const double& xc, const double& yc, const double& r);
/* calculate mean and sigma of points */
void calculateMeanSigma(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, double& mean, double& sigma);
#endif
