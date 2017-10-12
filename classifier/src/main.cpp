#include "samaritain.h"
#include "facerec.h"

int main() {
   FaceRecognition faceRecognition;
   faceRecognition.setConfig("./data/dlib_models/shape_predictor_5_face_landmarks.dat", "./data/dlib_models/dlib_face_recognition_resnet_model_v1.dat", "data/faces/boehm_s.jpg", "data/faces/boehm_e.jpg");
   faceRecognition.init();

   Samaritain samaritain;
   samaritain.setConfig("data/cfg/coco.data", "data/cfg/yolo.cfg", "data/yolo.weights", 0.24, .5);
   samaritain.run();
   return 0;
}
