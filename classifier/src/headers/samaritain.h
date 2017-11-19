#ifndef _SAMARITAIN_H_
#define _SAMARITAIN_H_


// FOR DARKNET
#ifdef GPU
    #include "cuda_runtime.h"
    #include "curand.h"
    #include "cublas_v2.h"
    #ifdef CUDNN
        #include "cudnn.h"
    #endif
#endif
#include "opencv2/opencv.hpp"

#if defined(__cplusplus)
extern "C" {
    #endif
    #include "./darknet.h"
    #if defined(__cplusplus)
}
#endif


// -------------
#include "./facerec.h"
#include "./postgresql.h"

extern "C" image ipl_to_image(IplImage* src);
extern "C" void ipl_into_image(IplImage* src, image im);

int free_stuff(image im, image sized, box *boxes, float **probs, ::layer l);


using namespace std;
using namespace cv;

class Samaritain {
public:
   char *datacfg;
   char *cfgfile;
   char *weightfile;
   float thresh;
   float hier_thresh;

   void setConfig(char *datacfg, char *cfgfile, char *weightfile, float thresh, float hier_thresh);
   int free_stuff(image im, image sized, box *boxes, float **probs, ::layer l);
   void show_coordinates(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes, Mat frame);

   void run(FaceRecognition faceRecognition,  PostgreSQL *db);
};

#endif
