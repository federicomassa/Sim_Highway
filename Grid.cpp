/*
  Implementation of class Grid
*/

#include <cmath>
#include <utility>

#include "Grid.h"
#include "utility.h"
#include "definition.h"
#include "List.h"
#include "Tensor4.h"
#include "Sensing.h"

using namespace std;

/* Constructor, takes frame width, height, margin percentage, number of grid lines in X and Y */
Grid::Grid(const int& W, const int& H, const double& e, const int& gridNX, const int& gridNY)
{
  init(W, H, e, gridNX, gridNY);  
}

void Grid::init(const int& W, const int& H, const double& e, const int& gridNX, const int& gridNY)
{
  frame = NULL;
  
  frameWidth = W;
  frameHeight = H;
  edge = e;
  
  int marginX = floor(double(frameWidth)*edge);
  int marginY = floor(double(frameHeight)*edge);
  
  gridXmin = marginX;
  gridXmax = frameWidth - marginX;

  gridYmin = marginY;
  gridYmax = frameHeight - marginY;

  frame = gdImageCreateTrueColor(frameWidth, frameHeight);
  int white = gdImageColorResolve(frame, 255, 255, 255);

  gdImageFill(frame, 0, 0, white);

  nX = gridNX;
  nY = gridNY;

  deltaX = (gridXmax - gridXmin)/nX;
  deltaY = (gridYmax - gridYmin)/nY;

  xMin = xMax = yMin = yMax = -1;
  
}

bool Grid::drawGrid()
{

  int black = gdImageColorResolve(frame, 0, 0, 0);

  for (int xi = 0; xi <= nX; xi++)
    gdImageFilledRectangle(frame, gridXmin + deltaX*xi, gridYmin, gridXmin + deltaX*xi + gridWidth, gridYmax, black);

  for (int yi = 0; yi <= nY; yi++)
    gdImageFilledRectangle(frame, gridXmin, gridYmin + deltaY*yi, gridXmax, gridYmin + deltaY*yi + gridWidth, black);


  
  return true;
}

bool Grid::drawAxes(const int& nTicksX, const int& nTicksY)
{

  int tickColor = gdImageColorResolve(frame, 153, 153, 153);


  /* Draw axes, +- 1 pixel to avoid overlapping with previous rectangle */
  
  /* x axis ticks */
  for (int xi = 0; xi <= nX; xi++)
    for (int t = 0; t < nTicksX; t++)
      {
	if (xi < nX)
	  gdImageFilledRectangle(frame,
				 gridXmin + deltaX*xi + deltaX/nTicksX*t, gridYmax + 1,
				 gridXmin + deltaX*xi + deltaX/nTicksX*t + tickWidth, gridYmax + 1 + tickLength, tickColor);
	else //after last tick do not draw secondary ticks
	  {
	    gdImageFilledRectangle(frame,
				   gridXmin + deltaX*xi, gridYmax + 1,
				   gridXmin + deltaX*xi + tickWidth, gridYmax + 1 + tickLength, tickColor);
	    break;
	  }
      }


  /* y axis ticks */
  for (int yi = 0; yi <= nY; yi++)
    for (int t = 0; t < nTicksY; t++)
      {
	if (yi < nY)
	  gdImageFilledRectangle(frame,
				 gridXmin - tickLength - 1, gridYmin + deltaY*yi + deltaY/nTicksY*t,
				 gridXmin - 1, gridYmin + deltaY*yi + deltaY/nTicksY*t + tickWidth, tickColor);
	else //after last tick do not draw secondary ticks
	  {
	    gdImageFilledRectangle(frame,
				   gridXmin - tickLength - 1, gridYmin + deltaY*yi,
				   gridXmin - 1, gridYmin + deltaY*yi + tickWidth, tickColor);
	    break;
	  }
	
      }

  
  return true;
  
}

bool Grid::drawLabels(const string& xlabel, const double& xmin, const double& xmax,
		      const string& ylabel, const double& ymin, const double& ymax)
{
  
  
  xMin = xmin;
  xMax = xmax;

  yMin = ymin;
  yMax = ymax;
  
  int bounds[8];
  int labelColor = gdImageColorResolve(frame, 0, 0, 0);
  int labelSize = 20;
  int textSize = 14;
  
  /*===========  X LABEL ============ */
  int distanceFromEdgeX = gridYmin/8;
  
  /* First, do not draw string to measure string bounds */
  gdImageStringFT(NULL, bounds, labelColor, FONT_NAME, labelSize, 0,
		  0, 0, const_cast<char*>(xlabel.c_str()));
  /* Draw string on frame */  
  
  gdImageStringFT(frame, bounds, labelColor, FONT_NAME, labelSize, 0,
		  gridXmax - (bounds[2] - bounds[6]), frameHeight - distanceFromEdgeX, const_cast<char*>(xlabel.c_str()));

  
  // Draw numbers on axis
  for (int xi = 0; xi <= nX; xi++)
    {
      // write numbers every two lines
      if (xi % 2 != 0)
	continue;
      
      double x = xmin + (xmax - xmin)/nX*xi;
      string xs = toString(x, 2);
      
      gdImageStringFT(NULL, bounds, labelColor, FONT_NAME, textSize, 0,
		      0, 0, const_cast<char*>(xs.c_str()));
      gdImageStringFT(frame, bounds, labelColor, FONT_NAME, textSize, 0,
		      gridXmin + deltaX*xi - (bounds[2] - bounds[6])/2, gridYmax + tickLength + 5 + (bounds[3] - bounds[7]), const_cast<char*>(xs.c_str()));
    }

  
  /*===========  Y LABEL ============ */
  int distanceFromEdgeY = gridXmin/8;
  
  /* First, do not draw string to measure string bounds */
  gdImageStringFT(NULL, bounds, labelColor, FONT_NAME, labelSize, 0,
		  0, 0, const_cast<char*>(ylabel.c_str()));
  /* Draw string on frame */
  gdImageStringFT(frame, bounds, labelColor, FONT_NAME, labelSize, 0.5*PI,
		  bounds[3] - bounds[7] + distanceFromEdgeY, gridYmin + (bounds[2] - bounds[6]), const_cast<char*>(ylabel.c_str()));

  // Draw numbers on axis
  for (int yi = 0; yi <= nY; yi++)
    {
      // Draw numbers every two lines
      if (yi % 2 != 0)
	continue;
      
      double y = ymin + (ymax - ymin)/nY*yi;
      string ys = toString(y, 2);
      gdImageStringFT(NULL, bounds, labelColor, FONT_NAME, textSize, 0,
		      0, 0, const_cast<char*>(ys.c_str()));
      gdImageStringFT(frame, bounds, labelColor, FONT_NAME, textSize, 0,
		      gridXmin - tickLength - 5 - (bounds[2] - bounds[6]) , gridYmax - deltaY*yi + (bounds[3] - bounds[7])/2, const_cast<char*>(ys.c_str()));
    }

  
  
  return true;
}

void Grid::fillCell(const int& xi, const int& yi)
{
  int cellColor = gdImageColorResolve(frame, 0, 0, 255);

  //Filling cell, +- 1 pixel to not overlap with previous rectangle
  gdImageFilledRectangle(frame,
			 gridXmin + deltaX*xi + gridWidth + 1, gridYmin + deltaY*yi + gridWidth + 1,
			 gridXmin + deltaX*(xi + 1) - 1, gridYmin + deltaY*(yi + 1) - 1, cellColor);
}

/* Call after drawLabels */
void Grid::fillCell(const double& x, const double& y)
{
  
  if (xMin == xMax || yMin == yMax)
    error("Grid::fillCell", "First call drawLabels");

  if (x < xMin || x > xMax || y < yMin || y > yMax)
    error("Grid::fillCell", "x,y out of range specified in drawLabels");

  int xi = floor((x - xMin)/(xMax - xMin)*nX);
  int yi = floor((y - yMin)/(yMax - yMin)*nY);

  fillCell(xi, yi);
  
}

string Grid::save(const string prefix, const string suffix)
{
  if (frame == NULL)
    error("Grid", "Cannot save on null file");

  fileName = "ProvaGrid/" + prefix + suffix + ".png";

  FILE* out;
  /* save to disk */
  out = fopen(fileName.c_str(), "wb");
  /* error handling */
  if (out == NULL)
    error("Image::save", "Cannot write on file: " + fileName);
  gdImagePng(frame, out);
  fclose(out);

  return fileName;
}

/*
  Draw two state variables on a grid

   list = list of rectangles, each containing a tensor of sensing
   i1, i2 = indexes of the pair of state variables that we want to show in a grid (in the order x, y, theta, v)
*/
void Grid::drawStateSpace(const List<Tensor4<Sensing> >& list, const int& i1, const int& i2)
{

  /* error handling */
  if (i1 < 0 || i1 > 3 || i2 < 0 || i2 > 3)
    error("Grid::drawStateSpace", "Indexes must lie between 0 and 3");
  
  
  Iterator<Tensor4<Sensing> > iter(list);
  Tensor4<Sensing> tens;

  double var1 = 0, var2 = 0;
  double var1Min = 1000000;
  double var1Max = -1000000;
  double var2Min = 1000000;
  double var2Max = -1000000;

  /* Store points into a list */
  List<pair<double, double> > points;

  /* First compute every point to decide grid size */
  while(iter(tens))
    {
      for (int i = 0; i < tens.Dim1; i++)
	for (int j = 0; j < tens.Dim2; j++)
	  for (int k = 0; k < tens.Dim3; k++)
	    for (int l = 0; l < tens.Dim4; l++)
	      {
		Sensing tmpS = tens(i,j,k,l);

		switch (i1)
		  {
		  case 0:
		    var1 = tmpS.q.x;
		    break;
		  case 1:
		    var1 = tmpS.q.y;
		    break;
		  case 2:
		    var1 = tmpS.q.theta;
		    break;
		  case 3:
		    var1 = tmpS.q.v;
		    break;
		  }

		switch (i2)
		  {
		  case 0:
		    var2 = tmpS.q.x;
		    break;
		  case 1:
		    var2 = tmpS.q.y;
		    break;
		  case 2:
		    var2 = tmpS.q.theta;
		    break;
		  case 3:
		    var2 = tmpS.q.v;
		    break;
		  }

		points.insTail(make_pair(var1, var2));

		if (var1 < var1Min)
		  var1Min = var1;
		if (var1 > var1Max)
		  var1Max = var1;
		if (var2 < var2Min)
		  var2Min = var2;
		if (var2 > var2Max)
		  var2Max = var2;

      	      }
    }

  /* ==== Now create grid ==== */
  
  /* 10% margin of the maximum value */
  double xmin = setPrecision(var1Min - 0.1*var1Max, 2);
  double xmax = setPrecision(var1Max + 0.1*var1Max, 2);

  double ymin = setPrecision(var2Min - 0.1*var2Max, 2);
  double ymax = setPrecision(var2Max + 0.1*var2Max, 2);

  /* Init grid: 900x900, 10% white margin, 20x20 grid */
  init(900, 900, 0.1, 20, 20);

  drawGrid();
  drawAxes();
  
  string xlabel = indexToStateVar(i1);
  string ylabel = indexToStateVar(i2);
  
  drawLabels(xlabel, xmin, xmax, ylabel, ymin, ymax);

  
  /* Now actually fill cells */
  Iterator<pair<double, double> > icells(points);
  pair<double,double> tmpPair;

  while (icells(tmpPair))
    {
      fillCell(tmpPair.first, tmpPair.second);
    }
  
}

string indexToStateVar(const int& i)
{
  if (i == 0)
    return "x";
  else if (i == 1)
    return "y";
  else if (i == 2)
    return "theta";
  else if (i == 3)
    return "v";

  return "";
  
}
