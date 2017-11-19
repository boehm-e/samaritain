#ifndef _VIDEOSTREAM_H_
#define _VIDEOSTREAM_H_

#include "opencv2/opencv.hpp"
using namespace cv;

class Stream {
public:
   Stream(int camindex);
   ~Stream();

   VideoCapture cap;

   Mat getFrame();
   void loop();
};

#endif
