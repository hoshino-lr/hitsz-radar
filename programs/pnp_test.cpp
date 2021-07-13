//
// Created by hoshino on 2021/4/2.
//
#include <glog/logging.h>
#include "opencv2/opencv.hpp"
#include "armor_finder/armor_finder.h"
#include <rmconfig.h>
#include <rmutils.h>

//串口实例
// ArmorFinder实例
ArmorFinder *armor_finder;
rm::utils::rm_time *rmtime = new rm::utils::rm_time;
rm::config::general *config = new rm::config::general;
//运行时原图实例
cv::Mat src;
void loop();

void init();


int main(int argc, char **argv) {
  init();
  loop();
  return 0;
}


void init() {
      rmtime->init();
      config->init_from_file();
      armor_finder = new ArmorFinder(config->ENEMY_COLOR);
}
void loop() {
    cv::VideoCapture video = cv::VideoCapture("video/test.avi");
    cv::Mat map = cv::imread("radar.png",1);
    if (config->use_video)
    {
      while (video.isOpened())
      {
        video >> src;
        armor_finder->run(src,map);

      }
    }
}
