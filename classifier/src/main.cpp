#include "samaritain.h"

int main() {
   Samaritain samaritain;
   samaritain.setConfig("data/cfg/coco.data", "data/cfg/yolo.cfg", "data/yolo.weights", 0.24, .5);
   samaritain.run();
   return 0;
}
