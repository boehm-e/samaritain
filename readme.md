## Goal of the project

The goal of this project is to detect dangerous behaviour from video feeds, like fire, guns...
To do this we are using the darknet library.

## Prerequistes

You have two choices to compile this program:

1. with CUDA
  The program will run faster (16gb ram / gtx 960 2gb => 15fps)

2. without CUDA
  The program will run slower (16gb ram / gtx 960 2gb => < 1fps)

so we advise you to  install CUDA (we are using cuda 8.0)
##### To install cuda :
[Linux](https://doc.ubuntu-fr.org/cuda)
[Windows](https://developer.nvidia.com/cuda-downloads)

##### To install opencv2 :
`In both cases (CUDA || !CUDA) you will need to install opencv`
we are using opencv2 so to install it :

[Linux](https://doc.ubuntu-fr.org/opencv)
[Mac](https://jjyap.wordpress.com/2014/05/24/installing-opencv-2-4-9-on-mac-osx-with-python-support/)
[Windows](http://docs.opencv.org/2.4/doc/tutorials/introduction/windows_install/windows_install.html)

---

## Compile

cd darknet
emacs -nw Makefile
    GPU=1
    CUDNN=1
    OPENCV=1
    DEBUG=0

make

cd ../classifier
make run
