/* 
   AUTHOR = FEDERICO MASSA, ADRIANO FAGIOLINI
   DATE = 17.01.2017
   DESCRIPTION = Class to display a grid representing the state variables space (two by two). Taken partially by Image.h
*/
#ifndef ARC_H
#define ARC_H

#include <gd.h>
#include <string>

class Arc
{
  gdImagePtr frame;

  /* Width and height of frame in pixels */
  int frameWidth;
  int frameHeight;
  
  /* Initialize */
  void init();
 public:
  /* Default constructor */
  Arc(const int&, const int&);
  ~Arc();

  
  std::string save(const std::string&);
  /* Draw plain arc */
  bool drawArc(const int& endAngle = 90);
};

#endif
