#include "./samaritain.h"

void Samaritain::setConfig(char *datacfg, char *cfgfile, char *weightfile, float thresh, float hier_thresh) {
   this->datacfg = datacfg;
   this->cfgfile = cfgfile;
   this->weightfile = weightfile;
   this->thresh = thresh;
   this->hier_thresh = hier_thresh;
   cout << "dataCFG: " << this->datacfg << "\n";
}

int Samaritain::free_stuff(image im, image sized, box *boxes, float **probs, layer l) {
   free_image(im);
   free_image(sized);
   free(boxes);
   free_ptrs((void **)probs, l.w*l.h*l.n);
   return 0;
}

void Samaritain::show_coordinates(image im, int num, float thresh, box *boxes, float **probs, char **names, image **alphabet, int classes, Mat frame)
{
   for(int i = 0; i < num; ++i) {
      int _class = max_index(probs[i], classes);
      float prob = probs[i][_class];
      if(prob > this->thresh){
         int width = im.h * .006;

         if(0){
            width = pow(prob, 1./2.)*10+1;
            alphabet = 0;
         }
         box b = boxes[i];

         int left  = (b.x-b.w/2.)*im.w;
         int right = (b.x+b.w/2.)*im.w;
         int top   = (b.y-b.h/2.)*im.h;
         int bot   = (b.y+b.h/2.)*im.h;

         int _x, _y, _width, _height;

         cv::Size s = frame.size();
         int frameHeight = s.height;
         int frameWidth = s.width;

         _x = left;
         _y = top;
         _width = abs(left - right);
         _height = abs(top - bot);


         if (0 <= _x
            && 0 <= _width
            && _x + _width <= frameWidth
            && 0 <= _y
            && 0 <= _height
            && _y + _height <= frameHeight){
               // box within the image plane
               printf("DEBUG       (%s) : %d, %d, %d, %d \n", names[_class], _x, _y, _width, _height);
               namedWindow(names[_class],1);

               cv::Mat image(frame);
               cv::Rect myROI(_x, _y, _width, _height);
               cv::Mat croppedImage = image(myROI);

               imshow(names[_class], croppedImage);
            }
            else{
               printf("OUT OF RANGE BOX\n");
            }

         }
      }
   }

   void Samaritain::run()
   {
      ::list *options = read_data_cfg(this->datacfg);
      char *name_list = option_find_str(options, "names", "data/names.list");
      char **names = get_labels(name_list);

      image **alphabet = load_alphabet();
      network net = parse_network_cfg(this->cfgfile);
      if(this->weightfile) {
         load_weights(&net, this->weightfile);
      }
      set_batch_network(&net, 1);


      srand(2222222);
      int j;
      float nms=.3;


      // HANDLE WEBCAM
      VideoCapture cap(0);

      if(!cap.isOpened()) {
         return;
      }

      Mat frame;
      while(true) {
         double demo_time = what_time_is_it_now();

         cap >> frame;
         if(waitKey(1) >= 0) break;

         IplImage ipl_img = frame;
         image im = ipl_to_image(&ipl_img);
         image sized = letterbox_image(im, net.w, net.h);
         layer l = net.layers[net.n-1];

         box *boxes = (box*)calloc(l.w*l.h*l.n, sizeof(box));
         float **probs = (float**)calloc(l.w*l.h*l.n, sizeof(float *));

         for(j = 0; j < l.w*l.h*l.n; ++j) {
            probs[j] = (float*)calloc(l.classes + 1, sizeof(float *));
         }

         float **masks = 0;

         if (l.coords > 4) {
            masks = (float**)calloc(l.w*l.h*l.n, sizeof(float*));
            for(j = 0; j < l.w*l.h*l.n; ++j) {
               masks[j] = (float*)calloc(l.coords-4, sizeof(float *));
            }
         }

         float *X = sized.data;
         network_predict(net, X);
         get_region_boxes(l, im.w, im.h, net.w, net.h, this->thresh, probs, boxes, masks, 0, 0, this->hier_thresh, 1);
         if (nms) {
            do_nms_obj(boxes, probs, l.w*l.h*l.n, l.classes, nms);
         }

         draw_detections(im, l.w*l.h*l.n, this->thresh, boxes, probs, masks, names, alphabet, l.classes);
         show_image(im, "samaritain");
         // show_coordinates(im, l.w*l.h*l.n, this->thresh, boxes, probs, names, alphabet, l.classes, frame);
         free_stuff(im, sized, boxes, probs, l);
      }
   }

   int main() {
      return 0;
   }
