/* 
   AUTHOR = FEDERICO MASSA, ADRIANO FAGIOLINI
   DATE = 17.01.2017
   DESCRIPTION = Class to display a grid representing the state variables space (two by two). Taken partially by Image.h
*/
#ifndef GRID_H
#define GRID_H

#include <gd.h>
#include <string>

template <typename T>
class List;

template <typename T>
class Tensor5;

class Sensing;

const int gridWidth = 2;
const int tickWidth = 1;
const int tickLength = 8;
const double pi = 3.14159;

extern int now;

class Grid
{
  gdImagePtr frame;
  std::string fileName;

  /* Width and height of frame in pixels */
  int frameWidth;
  int frameHeight;

  /* margin percentage left at every side */
  double edge;

  // pixel coordinates of the grid
  int gridXmin, gridXmax, gridYmin, gridYmax;

  // real coordinates of the grid
  double xMin, xMax, yMin, yMax;
  
  // pixel spacing
  int deltaX, deltaY;

  // number of grid lines
  int nX, nY;
  
 public:
  /* Default constructor */
  Grid() {}
  /* Alternative constructor */
  Grid(const int&, const int&, const double&, const int&, const int&);
  ~Grid() {}

  /* Initialize */
  void init(const int&, const int&, const double&, const int&, const int&);
  
  /* Save to PNG file */
  std::string save(const std::string prefix, const std::string suffix);
  /* Draw plain grid */
  bool drawGrid();
  /* Draw axes on grid */
  bool drawAxes(const int& nTicksX = 1, const int& nTicksY = 1);
  /* Draw axes' label */
  bool drawLabels(const std::string&, const double&, const double&,
		  const std::string&, const double&, const double&);
  /* Fill a cell with xi,yi coordinates */
  void fillCell(const int& xi, const int& yi);

  /* Fill a cell corresponding to the points x, y --- Need xMin... initialized  */
  void fillCell(const double& x, const double& y);
  
  /* Represent a pair of state variables onto a grid, taken from a tensor */
  void drawStateSpace(const List<Tensor5<Sensing> >&, const int&, const int&);
  
};

std::string indexToStateVar(const int&);

#endif
