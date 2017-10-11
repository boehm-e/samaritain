## Goal of the project

The goal of this project is to detect dangerous behaviour from video feeds, like fire, guns...
To do this we are using the darknet library.

## Prerequistes

```
libpqxx
postgresql
CUDA
nvcc
opencv3
```

##### Install
###### cuda (8.0) :
- [Linux](https://doc.ubuntu-fr.org/cuda)<br>
- [Windows](https://developer.nvidia.com/cuda-downloads)

###### opencv (3.2) :
- clone the [github repo](https://github.com/opencv/opencv)
- cmake
- make
- make install

###### postgresql :
- install postgresql
- [create a db](https://www.cyberciti.biz/faq/howto-add-postgresql-user-account/)
  - username = samaritain
  - password = samaritain
  - database = samaritain

###### libpqxx :
  - install libpqxx-dev


---

## Compile

```
cd darknet
make

cd ../classifier
make run
```
