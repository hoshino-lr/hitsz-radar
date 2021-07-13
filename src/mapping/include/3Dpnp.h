//
// Created by hoshino on 2021/3/27.
//

#ifndef RADAR_3DPNP_H
#define RADAR_3DPNP_H
#include <opencv2/opencv.hpp>
#include <vector>
#include "rmcamera_config.h"
#include "rmconfig.h"
class Radarpnp {
public:
    bool status = true;
    const int picture_h = 658;
    const int picture_w = 1225;
    const int arena_l = 28;  //长 28m
    const int arena_w = 15; //宽 15m
    rm::config::camera camera_1;
    rm::config::camera camera_2;
    cv::Point2d radar_solvePnp(cv::Rect2d &aim_coordinate_camara,rm::config::camera& camera_) const;
    cv::Point2d radar_rotate(cv::Mat &Tvec,rm::config::camera& camera_) const;
    Radarpnp(const rm::config::camera camera1,const rm::config::camera camera2):
    camera_1(camera1),camera_2(camera2)
    {}
//    bool coordinate_judge(cv::Mat &Tvec,double angle_x,double angle_y,double angle_z) const;
};
#endif //RADAR_3DPNP_H