#include "./facerec.h"

void FaceRecognition::setConfig(char *shape_predictor, char *net, char *firstFacePath, char *secondFacePath) {
  this->shape_predictor = shape_predictor;
  this->net = net;
  this->firstFacePath = firstFacePath;
  this->secondFacePath = secondFacePath;
}

int FaceRecognition::init() {
    try {
        frontal_face_detector detector = get_frontal_face_detector();
        dlib::shape_predictor sp;
        // deserialize("./data/dlib_models/shape_predictor_5_face_landmarks.dat") >> sp;
        deserialize(this->shape_predictor) >> sp;
        anet_type net;
        // deserialize("./data/dlib_models/dlib_face_recognition_resnet_model_v1.dat") >> net;
        deserialize(this->net) >> net;


        matrix<rgb_pixel> firstimg;
        // cv::Mat firstPerson = cv::imread(argv[1]);
        cv::Mat firstPerson = cv::imread(this->firstFacePath);
        dlib::assign_image(firstimg, dlib::cv_image<rgb_pixel>(firstPerson));

        matrix<rgb_pixel> secondimg;
        // cv::Mat secondPerson = cv::imread(argv[2]);
        cv::Mat secondPerson = cv::imread(this->secondFacePath);
        dlib::assign_image(secondimg, dlib::cv_image<rgb_pixel>(secondPerson));


        std::vector<matrix<rgb_pixel>> faces;
        for (auto face : detector(firstimg)) {
            auto shape = sp(firstimg, face);
            matrix<rgb_pixel> face_chip;
            extract_image_chip(firstimg, get_face_chip_details(shape,150,0.25), face_chip);
            faces.push_back(move(face_chip));
        }

        for (auto face : detector(secondimg)) {
            auto shape = sp(secondimg, face);
            matrix<rgb_pixel> face_chip;
            extract_image_chip(secondimg, get_face_chip_details(shape,150,0.25), face_chip);
            faces.push_back(move(face_chip));
        }

        if (faces.size() != 2) {
            cout << "More or less than two faces found (" << faces.size() << ")" << endl;
            return 1;
        }

        std::vector<matrix<float,0,1>> face_descriptors = net(faces);

        float diff = length(face_descriptors[0]-face_descriptors[1]);
        cout << "DIFFERENCE : " << diff << endl;
        if (diff < 0.5) {
            cout << "SAME FACE" << endl;
        } else {
            cout << "NOT SAME FACE" << endl;
        }
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
