/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Class inheriting from VideoCreator that create action manager demonstration videos
*/

#ifndef ACTION_VIDEO_CREATOR_H
#define ACTION_VIDEO_CREATOR_H

#include "VideoCreator.h"
#include "State.h"

class Environment;

class ActionVideoCreator : public VideoCreator
{
 private:
  const Environment* env;
  const int observerID;
  const int monitorID;

  State leftTriggerState;
  int leftTriggerTime;
  State leftEndState;
  int leftEndTime;

  List<State> monitorTrajectory;
  
  bool leftFound;
  bool leftEnded;
 public:
  /* constructor with observer and monitor vehicle's ID */
  ActionVideoCreator(const int&, const int&);
  
  /* Add a frame to the video following certain rules. 
     Each frame has to be created, saved, and then added with VideoCreator::addFrame(fileName).
     It has to be initialized first, running VideoCreator::init(...)*/
  void run();

  /* set environment */
  void setEnvironment(const Environment* e) {env = e;}
};

#endif
