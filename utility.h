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

void circle3points(const double&, const double&,
		   const double&, const double&,
		   const double&, const double&,
		   double&, double&, double&);

/* split string into elements separated by a delimiter */
std::vector<std::string> split(const std::string&, const std::string&);

#endif
