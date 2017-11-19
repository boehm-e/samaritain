#include "./headers/videostreams.h"
using namespace cv;

Stream::Stream(int camindex) {
  this->cap = VideoCapture(camindex);

  if(!cap.isOpened()) {
  }
}

Mat Stream::getFrame() {
  Mat frame;
  this->cap >> frame;
  return frame;
}

void Stream::loop() {
  while (1) {
    Mat frame = this->getFrame();
    imshow("Display window", frame);
    waitKey(1);
  }
}

int main() {
  Stream *strm = new Stream(0);
  strm->loop();
}
