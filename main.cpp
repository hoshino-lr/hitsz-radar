//
// Created by hoshino on 2021/4/2.
//
#include <glog/logging.h>
#include "opencv2/opencv.hpp"
#include "armor_finder/armor_finder.h"
#include <rmconfig.h>
#include <rmutils.h>
#include <rmcamera.h>
//串口实例
// ArmorFinder实例
ArmorFinder *armor_finder_1;
ArmorFinder *armor_finder_2;
rm::utils::rm_time *rmtime = new rm::utils::rm_time;
rm::config::general *config = new rm::config::general;
auto* camera1 = new rm::camera;
auto* camera2 = new rm::camera;
//运行时原图实例
cv::Mat src1;
cv::Mat src2;
void loop();

void init();


int main(int argc, char **argv) {
  if (argc == 1) google::InitGoogleLogging(*argv);
  LOG(INFO) << "Welcome to radar!";
  init();
  loop();
  return 0;
}

void window_init()
{
  cv::namedWindow("box1");
  cv::namedWindow("box2");
  cv::namedWindow("map");
  cv::moveWindow("box1",0,0);
  cv::moveWindow("box2",640,0);
  cv::moveWindow("map",800,600);
}

void init() {
  window_init();
  rmtime->init();
  config->init_from_file();
  armor_finder_1 = new ArmorFinder(config->ENEMY_COLOR,1);
  armor_finder_2 = new ArmorFinder(config->ENEMY_COLOR,2);
}
void loop() {
  cv::Mat map = cv::imread("radar.png",1);
  std::vector<cv::Point2d> boxes1;
  std::vector<cv::Point2d> boxes2;
  if (config->use_video)
  {
    cv::VideoCapture video = cv::VideoCapture("video/test.avi");
    while (video.isOpened())
    {
      video >> src1;
      armor_finder_1->run(src1,map);
      armor_finder_2->run(src1,map);
    }
  }
  else
  {
    camera1->open(&config->camConfig1,config->ARMOR_CAMERA_EXPOSURE,config->ARMOR_CAMERA_GAIN);
    camera2->open(&config->camConfig2,config->ENERGY_CAMERA_EXPOSURE,config->ENERGY_CAMERA_GAIN);

    while (camera1->is_open() or camera2->is_open())
    {
      if (camera1->is_open())
      {
        camera1->get_frame(src1);
        if(!src1.empty())
        {
          cv::resize(src1,src1,cv::Size(640,480));
          boxes1 = armor_finder_1->run(src1,map);
        }

      }
      if (camera2->is_open())
      {
        camera2->get_frame(src2);
        if (!src2.empty())
        {
          cv::resize(src2,src2,cv::Size(640,480));
          boxes2 = armor_finder_2->run(src2,map);
        }
      }
      point_dl(boxes1,boxes2,map);
    }
  }
}
