/*
  Implementation of class Grid
*/

#include <cmath>
#include "utility.h"

#include "Arc.h"

using namespace std;

/* Constructor, takes frame width, height, margin percentage, number of grid lines in X and Y */
Arc::Arc(const int& W, const int& H)
{
  frameWidth = W;
  frameHeight = H;
  
  frame = gdImageCreateTrueColor(frameWidth, frameHeight);
  
  init();
}

Arc::~Arc()
{
  gdImageDestroy(frame);
}
void Arc::init()
{ 
    

  int white = gdImageColorResolve(frame, 255, 255, 255);

  gdImageFill(frame, 0, 0, white);

}

bool Arc::drawArc(const int& endAngle)
{

  int black = gdImageColorResolve(frame, 0, 0, 0);
  int red = gdImageColorResolve(frame, 255, 0, 0);
  
  gdImageFilledArc(frame, frameWidth/2, frameHeight/2, frameWidth, frameHeight, 0, endAngle, red, gdPie);
  
  return true;
}

string Arc::save(const std::string& fileName)
{
  if (frame == NULL)
    error("Arc", "Cannot save on null file");

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
