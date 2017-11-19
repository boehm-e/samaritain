#include "headers/samaritain.h"
#include "headers/facerec.h"
#include "headers/postgresql.h"

#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/bind.hpp>

void loop(FaceRecognition faceRecognition) {
   while (true) {
      faceRecognition.compare("./data/faces/boehm_e.jpg", "./data/faces/boehm_s.jpg");
      boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
   }
}

void addPeoples(FaceRecognition faceRecognition, PostgreSQL *db) {

   dlib::matrix<float,0,1> threat_face_descriptor;
   threat_face_descriptor = faceRecognition.getFaceDescriptor_from_picture("./data/faces/boehm_s.jpg");
   db->addThreat("boehm_s", threat_face_descriptor);
   threat_face_descriptor = faceRecognition.getFaceDescriptor_from_picture("./data/faces/boehm_e.jpg");
   db->addThreat("boehm_e", threat_face_descriptor);
   threat_face_descriptor = faceRecognition.getFaceDescriptor_from_picture("./data/faces/fares_k.jpg");
   db->addThreat("fares_k", threat_face_descriptor);
}

int main() {
   PostgreSQL *db = new PostgreSQL("samaritain","samaritain","samaritain","127.0.0.1");
   boost::thread_group tgroup;

   // INIT FACE RECOGNITION
   FaceRecognition faceRecognition;
   faceRecognition.setConfig("./data/dlib_models/shape_predictor_5_face_landmarks.dat", "./data/dlib_models/dlib_face_recognition_resnet_model_v1.dat");
   faceRecognition.init();


   // INIT YOLO
   Samaritain samaritain;
   samaritain.setConfig("data/cfg/coco.data", "data/cfg/yolo.cfg", "data/yolo.weights", 0.24, .5);

   // CREATE THREADS TO MAKE THEM RUN SIDE BY SIDE
   // tgroup.create_thread( boost::bind( &loop, faceRecognition ) ); // faceRecognition.init();
   // tgroup.create_thread( boost::bind( &Samaritain::run, &samaritain ) ); // samaritain.run();

   samaritain.run(faceRecognition, db);

   addPeoples(faceRecognition, db);

   // START THE JOB
   // tgroup.join_all();

   return 0;
}
