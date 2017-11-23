#include "./headers/facerec.h"

void FaceRecognition::setConfig(char *shape_predictorFile, char *netFile) {
  this->shape_predictorFile = shape_predictorFile;
  this->netFile = netFile;
}

matrix<float,0,1> FaceRecognition::getFaceDescriptor_from_picture(char *facee) {
  matrix<rgb_pixel> firstimg;
  cv::Mat firstPerson = cv::imread(facee);
  dlib::assign_image(firstimg, dlib::cv_image<rgb_pixel>(firstPerson));


  std::vector<matrix<rgb_pixel>> faces;

  for (auto face : this->detector(firstimg)) {
    auto shape = this->sp(firstimg, face);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(firstimg, get_face_chip_details(shape,150,0.25), face_chip);
    faces.push_back(move(face_chip));
  }


  try {
    if (faces.size() != 1) {
      throw std::logic_error( "More or less than ONE face" );
    }
  }
  catch ( const std::exception & e ) {
    std::cerr << e.what();
  }



  matrix<float,0,1> face_descriptor = this->net(faces)[0];
  return face_descriptor;
}

void drawPoiDesignOnFaces(dlib::rectangle rect, cv::Mat frame, char *name) {
  dlib::point ctr = center(rect);
  int radius = ((rect.right() - rect.left()) / 2) * 1.5;
  Point center = Point(ctr.x(), ctr.y()*0.85);
  cout << "WIDTH : " << radius << " | X : " << center.x << " | Y : " << center.y << endl;


  int height = sqrt((radius*radius)) - sqrt((radius*2)*(radius*2));
  Point topLeft = Point((center.x - height), (center.y - 2*radius/3));
  Point topRight = Point((center.x + height), (center.y - 2*radius/3));
  Point bottom = Point((center.x), (center.y + radius * 4/3));


  circle(frame, center, radius, Scalar(150,150,150), 8); // grey
  circle(frame, center, radius+8, Scalar(50,50,50), 8); // black
  circle(frame, center, radius+4, Scalar(250,250,250), 1); // white
  line(frame, topLeft, topRight, Scalar(0,0,255), 2.5);
  line(frame, topRight, bottom, Scalar(0,0,255), 2.5);
  line(frame, topLeft, bottom, Scalar(0,0,255), 2.5);
  putText(frame, name, Point(center.x + radius, center.y + radius), FONT_HERSHEY_PLAIN, 1.5, CV_RGB(255, 255, 255), 2.0);
}

void FaceRecognition::findThreats_from_cvmat(cv::Mat video_frame_cv, PostgreSQL *db) {
  std::vector<matrix<float,0,1>> threats;
  threats = this->threats;

  std::vector<string> threats_names;
  threats_names = this->threats_names;

  matrix<rgb_pixel> video_frame;
  dlib::assign_image(video_frame, dlib::cv_image<rgb_pixel>(video_frame_cv));

  std::vector<matrix<rgb_pixel>> faces;
  std::vector<dlib::rectangle> face_boxes;

  for (auto face : this->detector(video_frame)) {
    face_boxes.push_back(face);
    auto shape = this->sp(video_frame, face);
    matrix<rgb_pixel> face_chip;
    extract_image_chip(video_frame, get_face_chip_details(shape,150,0.25), face_chip);
    faces.push_back(move(face_chip));
  }


  std::vector<matrix<float,0,1>> face_descriptors = this->net(faces);
  cout << "FOUND  " << faces.size() << " FACES" << endl;

  for (size_t i = 0; i < face_descriptors.size(); i++) { // for each faces found on picture
    for (size_t j = 0; j < threats.size(); j++) { // for each threats
      float diff = length(face_descriptors[i]-threats[j]);
      if (diff < this->threshold) {
        char *name = new char[threats_names[j].length() + 1];
        std::strcpy(name, threats_names[j].c_str());
        drawPoiDesignOnFaces(face_boxes[i], video_frame_cv, name);
        cout << "FOUND THREAT ["  << threats_names[j] << "] (" << diff << ")" << endl;
      }
    }
  }
  cout << "=========" << endl;
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

int FaceRecognition::init(PostgreSQL *db) {
  try {
    this->detector = get_frontal_face_detector();
    deserialize(this->shape_predictorFile) >> this->sp;
    deserialize(this->netFile) >> this->net;
    this->threats = db->getThreats();
    this->threats_names = db->getThreatsNames();

  } catch (std::exception& e) {
    cout << e.what() << endl;
  }
  return 0;
}

// std::vector<matrix<rgb_pixel>> jitter_image(const matrix<rgb_pixel>& img) {
//   thread_local random_cropper cropper;
//   cropper.set_chip_dims(150,150);
//   cropper.set_randomly_flip(true);
//   cropper.set_max_object_size(0.99999);
//   cropper.set_background_crops_fraction(0);
//   cropper.set_min_object_size(0.97);
//   cropper.set_translate_amount(0.02);
//   cropper.set_max_rotation_degrees(3);
//
//   std::vector<mmod_rect> raw_boxes(1), ignored_crop_boxes;
//   raw_boxes[0] = shrink_rect(get_rect(img),3);
//   std::vector<matrix<rgb_pixel>> crops;
//
//   matrix<rgb_pixel> temp;
//   for (int i = 0; i < 100; ++i) {
//     cropper(img, raw_boxes, temp, ignored_crop_boxes);
//     crops.push_back(move(temp));
//   }
//   return crops;
// }
