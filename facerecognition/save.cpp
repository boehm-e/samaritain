#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace dlib;
using namespace std;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
alevel0<
alevel1<
alevel2<
alevel3<
alevel4<
max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
input_rgb_image_sized<150>
>>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------

std::vector<matrix<rgb_pixel>> jitter_image(
    const matrix<rgb_pixel>& img
);

// ----------------------------------------------------------------------------------------

int main(int argc, char** argv) try
{

    // The first thing we are going to do is load all our models.  First, since we need to
    // find faces in the image we will need a face detector:
    frontal_face_detector detector = get_frontal_face_detector();
    // We will also use a face landmarking model to align faces to a standard pose:  (see face_landmark_detection_ex.cpp for an introduction)
    shape_predictor sp;
    deserialize("data/shape_predictor_5_face_landmarks.dat") >> sp;
    // And finally we load the DNN responsible for face recognition.
    anet_type net;
    deserialize("data/dlib_face_recognition_resnet_model_v1.dat") >> net;

    // convert cvimage (cv::Mat) to dlibimage (dlib::cv_image<bgr_pixel))
    // cv::namedWindow( "Display window", WINDOW_AUTOSIZE );
    // cv::imshow( "Display window", cvimage );






    // Display the raw image on the screen
    // image_window win(img);

    // Run the face detector on the image of our action heroes, and for each face extract a
    // copy that has been normalized to 150x150 pixels in size and appropriately rotated
    // and centered.

    // array of matrix<rgb_pixel>

    cv::Mat firstPerson = cv::imread(argv[1]);
    cv::Mat secondPerson = cv::imread(argv[2]);


    Size size(150,150);
    Mat firstDst, firstSrc;

    cv::resize(firstPerson,firstDst,size);//resize image
    matrix<rgb_pixel> pictureFirstFace;
    dlib::assign_image(pictureFirstFace, dlib::cv_image<rgb_pixel>(firstDst));

    Mat secondDst, secondSrc;
    cv::resize(secondPerson,secondDst,size);//resize image
    matrix<rgb_pixel> pictureSecondFace;
    dlib::assign_image(pictureSecondFace, dlib::cv_image<rgb_pixel>(secondDst));


    std::vector<matrix<rgb_pixel>> faces;

    faces.push_back(pictureFirstFace);
    faces.push_back(pictureSecondFace);


    // for (auto face : detector(img))
    // {
    //     printf("FACE left : %ld \n", face.left());
    //     printf("FACE right : %ld \n", face.right());
    //     printf("FACE top : %ld \n", face.top());
    //     printf("FACE bottom : %ld \n\n", face.bottom());
    //
    //     // auto shape = sp(img, face);
    //     // matrix<rgb_pixel> face_chip;
    //     // extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
    //     // faces.push_back(move(face_chip));
    //
    //     // Also put some boxes on the faces so we can see that the detector is finding
    //     // them.
    //     win.add_overlay(face);
    // }

    if (faces.size() == 0)
    {
        cout << "No faces found in image!" << endl;
        return 1;
    }

    // This call asks the DNN to convert each face image in faces into a 128D vector.
    // In this 128D vector space, images from the same person will be close to each other
    // but vectors from different people will be far apart.  So we can use these vectors to
    // identify if a pair of images are from the same person or from different people.
    std::vector<matrix<float,0,1>> face_descriptors = net(faces);

    // Finally, let's print one of the face descriptors to the screen.
    cout << "face descriptor for FIRST face: " << trans(face_descriptors[0]) << endl;
    cout << "face descriptor for SECOND face: " << trans(face_descriptors[1]) << endl;


    matrix<float,0,1> diff = face_descriptors[0]-face_descriptors[1];
    float value = 0.0;
    for(int i = 0; i < diff.size(); i++) value += diff(i) * diff(i);
    value = sqrt(value);

    cout << value << endl;
    if (length(face_descriptors[0]-face_descriptors[1]) < 0.6) {
        cout << "SAME PEOPLE" << endl;
    } else {
        cout << "NOT PEOPLE" << endl;
    }


}
catch (std::exception& e)
{
    cout << e.what() << endl;
}

// ----------------------------------------------------------------------------------------

std::vector<matrix<rgb_pixel>> jitter_image(
    const matrix<rgb_pixel>& img
)
{
    // All this function does is make 100 copies of img, all slightly jittered by being
    // zoomed, rotated, and translated a little bit differently.
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
    for (int i = 0; i < 100; ++i)
    {
        cropper(img, raw_boxes, temp, ignored_crop_boxes);
        crops.push_back(move(temp));
    }
    return crops;
}

// ----------------------------------------------------------------------------------------
