//
// Created by root on 2021/7/3.
//
#include <glog/logging.h>
#include "opencv2/opencv.hpp"
#include <rmcamera.h>
#include "rmconfig.h"
rm::config::general *config = new rm::config::general;
rm::config::camera *camerafig = new rm::config::camera(1084.32,1084.05,339.53,231.71,-0.0876,0.32596,-0.08626,-0.002323,0.001002,0,-45,0,0,5);
auto* camera = new rm::camera;
int main(int argc, char **argv)
{
    config->init_from_file();
    cv::Mat SRC;
    camera->open(&config->camConfig1,config->ARMOR_CAMERA_EXPOSURE,config->ARMOR_CAMERA_GAIN);
    if (config->use_video)
    {

    }
    else{
      while (camera->is_open())
      {
        camera->get_frame(SRC);
        cv::resize(SRC,SRC,cv::Size(640,480));
        if (config->save_video)
        {
          saveVideos(SRC,"test");
        }
        cv::imshow("window",SRC);
        cv::waitKey(1);
      }
    }
}

