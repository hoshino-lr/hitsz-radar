//
// Created by Chiro on 2021/1/23.
//

#ifndef RADAR_CAMERA_H
#define RADAR_CAMERA_H
#include <radar.hpp>
#ifndef RADAR_USE_DEBUG_CAMERA
#include "camera/cam_wrapper.h"
#endif

#include <opencv2/opencv.hpp>
#include <rmcamera_config.h>
#include <rmconfig.h>

namespace rm {

class camera {
public:
#ifdef RADAR_USE_DEBUG_CAMERA
static cv::VideoCapture cap;

  static bool open(rm::config::camera *camConfig, int camera_exposure, int camera_gain) {
    open();
    return is_open();
  }

  static bool open() {
    cap.open(RADAR_DEBUG_CAMERA_ID);
    return is_open();
  }

//  inline bool is_open() const { return cap.isOpened(); }
  static inline bool is_open() { return true; }

  static void get_frame(cv::Mat &frame) {
    static cv::Mat map = cv::imread("target.png");;
    cap.read(frame);
    frame = map.clone();
  }

#else
  Camera *cam;

  bool open(rm::config::camera *camConfig, int camera_exposure, int camera_gain) {
    cam = new Camera(1, reinterpret_cast<rm::config::camera *>(camConfig));
    cam->init();
    cam->setParam(camera_exposure, camera_gain);
    cam->start();
    return this->is_open();
  }

  inline bool is_open() {
    return cam->init_is_successful();
  }

  void get_frame(cv::Mat &frame) {
    cam->read(frame);
#ifndef __unix__
    if (frame.size[0]) cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
#endif
  }
#endif
};
}
#endif //RADAR_CAMERA_H
