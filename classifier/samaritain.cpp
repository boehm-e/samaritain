#ifdef GPU
#define BLOCK 512

#include "cuda_runtime.h"
#include "curand.h"
#include "cublas_v2.h"

#ifdef CUDNN
#include "cudnn.h"
#endif
#endif



#if defined(__cplusplus)
extern "C" {
  #endif

  #include "darknet.h"

  #if defined(__cplusplus)
}
#endif

#include "opencv2/opencv.hpp"

using namespace cv;



// void run_detector(int argc, char **argv)
void run_detector2(char *datacfg, char *cfg, char *weights, char *filename, float thresh, float hier_thresh, char *outfile, int fullscreen, int argc, char **argv)
{
  char *prefix = find_char_arg(argc, argv, "-prefix", 0);
  int cam_index = find_int_arg(argc, argv, "-c", 0);
  int frame_skip = find_int_arg(argc, argv, "-s", 0);
  int avg = find_int_arg(argc, argv, "-avg", 3);

  char *gpu_list = find_char_arg(argc, argv, "-gpus", 0);
  // char *outfile = find_char_arg(argc, argv, "-out", 0);
  int *gpus = 0;
  int gpu = 0;
  int ngpus = 0;
  if(gpu_list){
    printf("%s\n", gpu_list);
    int len = strlen(gpu_list);
    ngpus = 1;
    int i;
    for(i = 0; i < len; ++i){
      if (gpu_list[i] == ',') ++ngpus;
    }
    gpus = (int *)calloc(ngpus, sizeof(int));
    for(i = 0; i < ngpus; ++i){
      gpus[i] = atoi(gpu_list);
      gpu_list = strchr(gpu_list, ',')+1;
    }
  } else {
    gpu = gpu_index;
    gpus = &gpu;
    ngpus = 1;
  }

  int clear = find_arg(argc, argv, "-clear");
  int width = find_int_arg(argc, argv, "-w", 0);
  int height = find_int_arg(argc, argv, "-h", 0);
  int fps = find_int_arg(argc, argv, "-fps", 0);

  list *options = read_data_cfg(datacfg);
  int classes = option_find_int(options, "classes", 20);
  char *name_list = option_find_str(options, "names", "data/names.list");
  char **names = get_labels(name_list);
  demo(cfg, weights, thresh, cam_index, filename, names, classes, frame_skip, prefix, avg, hier_thresh, width, height, fps, fullscreen);
}

int main(int argc, char **argv)
{
  char *outfile = find_char_arg(argc, argv, "-out", 0);
  int fullscreen = find_arg(argc, argv, "-fullscreen");
  run_detector2("cfg/coco.data", "cfg/yolo.cfg", "yolo.weights", 0, .24, .5, outfile, fullscreen, argc, argv);
  return 0;
}
