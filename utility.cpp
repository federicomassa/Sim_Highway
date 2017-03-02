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

const double largeNumber = 1E10;
const double smallNumber = 1E-10;
/* calculate parameters of a circle passing through 3 given points. The radius has a sign depending on the curvature. 
 If p1 is the oldest point of the trajectory and p3 the most recent, if the vehicle is turning left the radius is positive. */
void circle3points(const double& x1, const double& y1,
		   const double& x2, const double& y2,
		   const double& x3, const double& y3,
		   double& centerX, double& centerY, double& radius)
{
  //check collinearity
  double m1, m2;
  double sign = 0;
  
  if (fabs(x2 - x1) > smallNumber && fabs(x3 - x2) > smallNumber)
    {
      m1 = (y3 - y2)/(x3 - x2);
      m2 = (y2 - y1)/(x2 - x1);

      if (m1 > m2)
	sign = 1;
      else
	sign = -1;
      
    }
  else if (fabs(y2 - y1) > smallNumber && fabs(y3 - y2) > smallNumber)
    {
      m1 = (x3 - x2)/(y3 - y2);
      m2 = (x2 - x1)/(y2 - y1);

      if (m1 > m2)
	sign = -1;
      else
	sign = 1;
      
    }
  else
    {
      std::cout << "ERROR circle3points: coincident points?" << std::endl;
      exit(1);
    }

  if (fabs(m1 - m2) < smallNumber)
    {
      centerX = (x1 + x2 + x3)/3.0;
      centerY = (y1 + y2 + y3)/3.0;
      radius = largeNumber;
      return;
    }

  
  /* equation of a circle passing through 3 points */
  double a1 = 2*(x2 - x1);
  double b1 = 2*(y2 - y1);
  double c1 = pow(x1, 2) - pow(x2, 2) + pow(y1, 2) - pow(y2, 2);
  
  double a2 = 2*(x3 - x1);
  double b2 = 2*(y3 - y1);
  double c2 = pow(x1, 2) - pow(x3, 2) + pow(y1, 2) - pow(y3, 2);
  
  centerX = -(b1*c2 - b2*c1)/(b1*a2 - b2*a1);
  centerY = -(a1*c2 - a2*c1)/(a1*b2 - a2*b1);
  radius = sqrt(pow(x1 - centerX, 2) + pow(y1 - centerY,2));

  radius *= sign;
  
}
