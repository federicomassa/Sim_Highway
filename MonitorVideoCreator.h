/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Class inheriting from VideoCreator that create monitor manager demonstration videos
*/

#ifndef MONITOR_VIDEO_CREATOR_H
#define MONITOR_VIDEO_CREATOR_H

#include "VideoCreator.h"

class Environment;

class MonitorVideoCreator : public VideoCreator
{
 private:
  const Environment* env;
  const Environment* globalEnv;
  const int observerID;
  const int monitorID;

 public:
  /* constructor with observer and monitor vehicle's ID */
  MonitorVideoCreator(const int&, const int&);
  
  /* Add a frame to the video following certain rules. 
     Each frame has to be created, saved, and then added with VideoCreator::addFrame(fileName).
     It has to be initialized first, running VideoCreator::init(...)*/
  void run();

  /* set environment */
  void setEnvironment(const Environment* e) {env = e;}
  void setGlobalEnvironment(const Environment* e) {globalEnv = e;}
};

#endif
