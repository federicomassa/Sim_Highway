/* 
   AUTHOR = Federico Massa, Adriano Fagiolini
   EMAIL = fedemassa91@gmail.com
   DESCRIPTION = Class to help creating videos
*/

#ifndef VIDEO_CREATOR_H
#define VIDEO_CREATOR_H

#include <cstddef>

class CvVideoWriter;
class CvSize;

class VideoCreator
{
 private:
  CvVideoWriter* video;

 protected:
  /* add a frame to the video from a stored image */

  void addFrame(const char*);
  bool isInitialized() {return (video != NULL);}
  int frameWidth, frameHeight;
  
 public:
  VideoCreator();
  virtual ~VideoCreator();

  /* create video */
  virtual void init(const char*, const int&, const double&, const CvSize&, const bool&);

  virtual void run() = 0;
  
};

#endif
