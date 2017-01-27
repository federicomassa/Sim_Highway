/**
 * DESIGN:      Reputation/Trust System in Cooperative Multi-Agent Systems
 *
 * FILE:        Channel.h
 * DESCRIPTION: interface for the Channel class
 *
 * AUTHOR:      Marco Pellinacci
 * DATE:        June 9, 2007
 */

#include "utility.h"
#include <cstdlib>
#include <cmath>

using namespace std;

/**
 * this function gives information about a specific error and it terminates
 * the program
 */
void error(const string className, const string msg)
{
    cerr << endl << "ERROR - " << className << " : " << msg << endl;
    exit(1);
}

/**
 * this function is called by set_new_handler()
 */
void exaustedMem()
{
    cerr << endl << "Exausted memory!" << endl;
    exit(2);
}

/**
 * this function returns the sign (+1 or -1) of a double
 */
int sign(const double x)
{
    if(x >= 0)
        return 1;
    else
        return -1;
}

string toString(int i, int pad)
{
    stringstream ss;
    if(pad)
    {
        ss.fill('0');
        ss.width(pad);
    }
    string str;
    ss << i;
    ss >> str;
    return str;
}

string toStringWithPrecision(double i, int precision)
{
  /* copy value */
  double x = fabs(i);
  
  stringstream ss;
  string str;
  
  if (precision && x > 1E-9)
    {
      double logx = floor(log10(x));
      
      x = x/pow(10, logx + 1 - precision);
      x = round(x);
      x = x*pow(10, logx + 1 - precision);
      
      ss << x*i/fabs(i);
    }
  else
    ss << i;
  
  ss >> str;
  return str;
}

double setPrecision(const double& n, const int& precision)
{
  /* copy value */
  double x = fabs(n);
    
  if (precision && x > 1E-9)
    {
      double logx = floor(log10(x));
      
      x = x/pow(10, logx + 1 - precision);
      x = round(x);
      x = x*pow(10, logx + 1 - precision);
    }

  if (n < 0)
    return -x;
  else
    return x;
}

/* Round to the largest number which is < n with desired precision */
double floorPrecision(const double& n, const int& precision)
{
  /* copy value */
  double x = fabs(n);
    
  if (precision && x > 1E-9)
    {
      double logx = floor(log10(x));
      
      x = x/pow(10, logx + 1 - precision);
      if (n > 0)
	x = floor(x);
      else
	x = ceil(x);
      
      x = x*pow(10, logx + 1 - precision);
    }

  if (n < 0)
    return -x;
  else
    return x;
}

/* Round to the smallest number which is > n with desired precision */
double ceilPrecision(const double& n, const int& precision)
{
  /* copy value */
  double x = fabs(n);
    
  if (precision && x > 1E-9)
    {
      double logx = floor(log10(x));
      
      x = x/pow(10, logx + 1 - precision);
      if (n > 0)
	x = ceil(x);
      else
	x = floor(x);
      
      x = x*pow(10, logx + 1 - precision);
    }

  if (n < 0)
    return -x;
  else
    return x;
}
