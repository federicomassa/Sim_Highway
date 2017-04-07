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
#include "Vector.h"
#include "Sensing.h"
#include <cstdlib>
#include <cmath>


using namespace std;

/**
 * this function gives information about a specific error and it terminates
 * the program
 */
void error(const string className, const string msg, bool exc)
{
  if (exc)
    throw 1;
  
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
      error("circle3points" ,"coincident points?", true);
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

vector<string> split(const string& str, const string& delim)
{
  vector<string> tokens;
  size_t prev = 0, pos = 0;
      do
	{
	  pos = str.find(delim, prev);
	  if (pos == string::npos) pos = str.length();
	  string token = str.substr(prev, pos-prev);
	  if (!token.empty()) tokens.push_back(token);
	  prev = pos + delim.length();
	}
      while (pos < str.length() && prev < str.length());
      return tokens;
}

void circleFit(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, double& xc, double& yc, double& R)
{
  /* error handling */
  if (states == 0)
    error("utility::circleFit", "Passed a null pointer");
  if (states->length < n)
    error("utility::circleFit", "State vector is to small compared to the number of measurements required");
  if (n <= 0)
    error("utility::circleFit", "N must be > 0");

  
  /* first calculate some auxiliary quantities */
  double S_x = 0, S_y = 0, S_uu = 0, S_uv = 0, S_vv = 0, S_uuu = 0, S_uvv = 0, S_vuu = 0, S_vvv = 0;

  for (int i = 0; i < n; i++)
    {
      S_x += (*states)[i].x;
      S_y += (*states)[i].y;
    }

  for (int i = 0; i < n; i++)
    {
      double u = (*states)[i].x - S_x/n;
      double v = (*states)[i].y - S_y/n;
      S_uu += u*u;
      S_uv += u*v;
      S_vv += v*v;
      S_uuu += u*u*u;
      S_uvv += u*v*v;
      S_vuu += v*u*u;
      S_vvv += v*v*v;
    }

  double delta = S_uu*S_vv - S_uv*S_uv;
  double deltaU = ((S_uuu + S_uvv)*S_vv - (S_vvv + S_vuu)*S_uv)/2.0;
  double deltaV = ((S_vvv + S_vuu)*S_uu - (S_uuu + S_uvv)*S_uv)/2.0;

  if (delta == 0)
    throw (int)0;

  double uc = deltaU/delta;
  double vc = deltaV/delta;

  double alpha = uc*uc + vc*vc + (S_uu + S_vv)/(double)n;

  if (alpha < 0)
    error("utility::circleFit", "Alpha < 0?");
  
  xc = uc + S_x/n;
  yc = vc + S_y/n;
  R = sqrt(alpha);
  
}

double distanceFromCircle(const double& x0, const double& y0, const double& xc, const double& yc, const double& r)
{
  /* Compute minimum distance point. There are two solution to the intersection of a line with a circle, 
   the right one will be the one with minimum distance */
  double xi1, xi2, yi1, yi2;

  /* tan of line going from x0,y0 to the center of the circle */
  double m;
  if (x0 != xc)
    m = (y0 - yc)/(x0 -xc);
  else
    error("utility::distanceFromCircle", "Trying to compute distance with vertical line");

  xi1 = xc + sqrt(r*r/(1+m*m));
  xi2 = xc - sqrt(r*r/(1+m*m));

  yi1 = m*(xi1 - xc) + yc;
  yi2 = m*(xi2 - xc) + yc;

  /* compute distance */
  double d1, d2;

  d1 = sqrt((x0-xi1)*(x0-xi1) + (y0-yi1)*(y0-yi1));
  d2 = sqrt((x0-xi2)*(x0-xi2) + (y0-yi2)*(y0-yi2));

  if (d1 < d2)
    return d1;
  else
    return d2;
}

double fitDistance(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, const double& xc, const double& yc, const double& r)
{
  if (states == 0)
    error("utility::fitDistance", "Passed null pointer");

  double totalD = 0;
  
  for (int i = 0; i < n; i++)
    {
      totalD += distanceFromCircle((*states)[i].x, (*states)[i].y, xc, yc, r);
    }

  return totalD;
}

void calculateMeanSigma(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n, double& mean, double& sigma)
{
  if (n < 2)
    error("utility::calculateMeanSigma", "N must be >= 2");
  
  mean = 0;
  for (int i = 0; i < n; i++)
    mean += (*states)[i].y;

  mean = mean/n;


  sigma = 0;
  for (int i = 0; i < n; i++)
    sigma += pow(((*states)[i].y - mean),2);

  sigma = sqrt(sigma/(n-1));
 
}

double calculateAverageDeltaY(const Vector<Sensing, VEHICLE_MEMORY>* states, const int& n)
{
  
}
