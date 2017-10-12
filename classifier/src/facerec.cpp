#include "./facerec.h"

void FaceRecognition::setConfig(char *shape_predictorFile, char *netFile) {
  this->shape_predictorFile = shape_predictorFile;
  this->netFile = netFile;
}

int FaceRecognition::compare(char *face1, char *face2) {

  matrix<rgb_pixel> firstimg;
  cv::Mat firstPerson = cv::imread(face1);
  dlib::assign_image(firstimg, dlib::cv_image<rgb_pixel>(firstPerson));

  matrix<rgb_pixel> secondimg;
  cv::Mat secondPerson = cv::imread(face2);
  dlib::assign_image(secondimg, dlib::cv_image<rgb_pixel>(secondPerson));


  std::vector<matrix<rgb_pixel>> faces;
  for (auto face : this->detector(firstimg)) {
    auto shape = this->sp(firstimg, face);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(firstimg, get_face_chip_details(shape,150,0.25), face_chip);
    faces.push_back(move(face_chip));
  }

  for (auto face : this->detector(secondimg)) {
    auto shape = this->sp(secondimg, face);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(secondimg, get_face_chip_details(shape,150,0.25), face_chip);
    faces.push_back(move(face_chip));
  }

  if (faces.size() != 2) {
    cout << "More or less than two faces found (" << faces.size() << ")" << endl;
    return 1;
  }

  std::vector<matrix<float,0,1>> face_descriptors = this->net(faces);

  float diff = length(face_descriptors[0]-face_descriptors[1]);
  cout << "DIFFERENCE : " << diff << endl;
  if (diff < 0.5) {
    cout << "SAME FACE" << endl;
  } else {
    cout << "NOT SAME FACE" << endl;
  }


}

int FaceRecognition::init() {
  try {
    this->detector = get_frontal_face_detector();
    deserialize(this->shape_predictorFile) >> this->sp;
    deserialize(this->netFile) >> this->net;
  } catch (std::exception& e) {
    cout << e.what() << endl;
  }
}

std::vector<matrix<rgb_pixel>> jitter_image(const matrix<rgb_pixel>& img) {
  thread_local random_cropper cropper;
  cropper.set_chip_dims(150,150);
  cropper.set_randomly_flip(true);
  cropper.set_max_object_size(0.99999);
  cropper.set_background_crops_fraction(0);
  cropper.set_min_object_size(0.97);
  cropper.set_translate_amount(0.02);
  cropper.set_max_rotation_degrees(3);

  std::vector<mmod_rect> raw_boxes(1), ignored_crop_boxes;
  raw_boxes[0] = shrink_rect(get_rect(img),3);
  std::vector<matrix<rgb_pixel>> crops;

  matrix<rgb_pixel> temp;
  for (int i = 0; i < 100; ++i) {
    cropper(img, raw_boxes, temp, ignored_crop_boxes);
    crops.push_back(move(temp));
  }
  return crops;
}

// int main() {
//   FaceRecognition faceRecognition;
//   faceRecognition.setConfig("./data/dlib_models/shape_predictor_5_face_landmarks.dat", "./data/dlib_models/dlib_face_recognition_resnet_model_v1.dat", "data/faces/boehm_s.jpg", "data/faces/boehm_e.jpg");
//   faceRecognition.init();
//   return 0;
// }
