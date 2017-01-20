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

using namespace std;

/**
 * this function gives information about a specific error and it terminates
 * the program
 */
void error(const string className, const string msg);

/**
 * this function is called by set_new_handler()
 */
void exaustedMem();

/**
 * this function returns the sign (+1 or -1) of a double
 */
int sign(const double);

string toString(int i, int pad = 0);

string toString(double i, int precision = 0, int pad = 0);

double setPrecision(const double& x, const int& precision);

inline int size(int i) { return sizeof(i); }

inline int size(double d) { return sizeof(d); }

#endif
