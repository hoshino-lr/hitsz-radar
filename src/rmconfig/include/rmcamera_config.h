//
// Created by chiro on 2021/5/10.
//

#ifndef RADAR_RMCAMERA_CONFIG_H
#define RADAR_RMCAMERA_CONFIG_H

#include <cstdint>
#include <opencv2/opencv.hpp>

namespace rm {
namespace config {
class camera {
public:
  double fx;
  double fy;
  double cx;
  double cy;
  double k1;
  double k2;
  double p1;
  double p2;
  double k3;
  double FOCUS_PIXEL = 0;
  int64_t roi_width = 600;
  int64_t roi_height = 600;
  int64_t roi_offset_x = 0;
  int64_t roi_offset_y = 0;
  int yaw;//绕z轴
  int pitch;
  int roll;//绕x轴
  int coordinate_x;
  int coordinate_y;
  cv::Mat mtx, dist, mapx, mapy;
  camera(double f_x,double f_y,double c_x,double c_y,double k_1,double k_2,double k_3,double p_1,double p_2,int Yaw,int Pitch,int Roll,int x, int y):
    fx(f_x),fy(f_y),cx(c_x),cy(c_y),k1(k_1),k2(k_2),k3(k_3),p1(p_1),p2(p_2),yaw(Yaw),pitch(Pitch),roll(Roll)
  ,coordinate_x(x)
  ,coordinate_y(y)
    {}
  void init() {
//    double mtx_[]= {fx,0,cx,0,fy,cy,0,0,1};
//    this->mtx = cv::Mat(3, 3,CV_64FC1, mtx_);
//    double dist_[] = {k1, k2, p1, p2, k3};
//    this->dist = cv::Mat(1, 5,CV_64FC1, dist_);
    this->mtx = (cv::Mat_<double>(3, 3 )  <<fx,0,cx,0,fy,cy,0,0,1);
    this->dist = (cv::Mat_<double>(1, 5 ) <<k1, k2, p1, p2, k3);
    cv::initUndistortRectifyMap(mtx, dist, cv::noArray(), mtx,
                                cv::Size(roi_width, roi_height), CV_32FC1, mapx, mapy);
    FOCUS_PIXEL = (fx + fy) / 2;
  }

  void undistort(cv::Mat &frame) const {
    remap(frame, frame, mapx, mapy, cv::INTER_LINEAR);
  }
};
};
};

#endif //RADAR_RMCAMERA_CONFIG_H
