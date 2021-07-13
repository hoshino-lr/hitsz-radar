//
// Created by erc on 1/3/20.
//

#ifndef RM2020_CAM_DRIVER_CAMWRAPPER_H
#define RM2020_CAM_DRIVER_CAMWRAPPER_H

#include <thread>
#include "GxIAPI.h"
#include "DxImageProc.h"
#include <iostream>
#include "wrapper_head.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
//#include "rmconfig2.h"
#include "rmcamera_config.h"

//#ifndef _TYPEDEF_CAMERA_CONFIG
//#define _TYPEDEF_CAMERA_CONFIG
//typedef rm::config::camera rm::config::camera;
//#endif

//class rm::config::camera {
//public:
//  double fx = 811.72293646509797,
//          fy = 812.08878700531454,
//          cx = 303.92727183847512,
//          cy = 307.00937071223274,
//          k1 = -0.23008069888906263,
//          k2 = 0.53017452547602162,
//          p1 = -0.0044045373220139242,
//          p2 = -0.0026143012397772166,
//          k3 = -1.2392524907626943;
//  double FOCUS_PIXEL;
//  int64_t roi_width = 1280;
//  int64_t roi_height = 1024;
//  int64_t roi_offset_x = 0;
//  int64_t roi_offset_y = 0;
//
//  cv::Mat mtx, dist, mapx, mapy;
//
//  cv::Mat getCameraMatrix() const {
//    return mtx;
//  }
//
//  cv::Mat getDistCoeff() const {
//    return dist;
//  }
//
//  void init() {
//    mtx = (cv::Mat_<double>(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
//    dist = (cv::Mat_<double>(1, 5) << k1, k2, p1, p2, k3);
//    cv::initUndistortRectifyMap(mtx, dist, cv::noArray(), mtx,
//                                cv::Size(roi_width, roi_height), CV_32FC1, mapx, mapy);
//    FOCUS_PIXEL = (fx + fy) / 2;
//  }
//
//  void undistort(cv::Mat &frame) const {
//    remap(frame, frame, mapx, mapy, cv::INTER_LINEAR);
//  }
//};

class Camera : public WrapperHead {
  friend void getRGBImage(Camera *p_cam);

private:
  std::string sn;
  int index;
  int exposure, gain;
  GX_STATUS status;

  int64_t nPayLoadSize;
  uint32_t nDeviceNum;

  GX_DEV_HANDLE g_hDevice;
  GX_FRAME_DATA g_frameData;
  int64_t g_nPixelFormat;
  int64_t g_nColorFilter;
  int64_t g_SensorHeight;
  int64_t g_SensorWidth;
  rm::config::camera camConfig;
  cv::Mat p_img;
  void *g_pRGBframeData;
  void *g_pRaw8Buffer;
  bool thread_running;
  bool init_success;

public:
  Camera(int idx,
         rm::config::camera *config);              // constructor, p_img is a pointer towards a 640*640 8uc3 Mat type
  ~Camera() override;

  bool init() final;                                        // init camera lib and do settings, be called firstly
  void setParam(int exposureInput, int gainInput);  // set exposure and gain
  void start();                              // start video stream
  void stop();                              // stop receiving frames
//  void calcRoi(); //autmatic resize parameters
  bool init_is_successful();                    // return video is available or not
  bool read(cv::Mat &src) final;
};

#endif //RM2020_CAM_DRIVER_CAMWRAPPER_H
