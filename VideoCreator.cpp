/* Implementation of class VideoCreator */

#include "VideoCreator.h"
#include <opencv2/highgui/highgui_c.h>

VideoCreator::VideoCreator()
{
  video = 0;
}

VideoCreator::~VideoCreator()
{
  if (video)
    cvReleaseVideoWriter(&video);
}

void VideoCreator::init(const char* fileName,
			const int& fourcc,
			const double& fps,
			const CvSize& frameSize,
			const bool& isColor)
{
  video = cvCreateVideoWriter(fileName, fourcc, fps, frameSize, isColor);
  frameWidth = frameSize.width;
  frameHeight = frameSize.height;
}

void VideoCreator::addFrame(const char* imageFile)
{
  IplImage* cvImg = cvLoadImage(imageFile);
  cvWriteFrame(video, cvImg);
  cvReleaseImage(&cvImg);
}
