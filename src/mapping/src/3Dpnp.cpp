//
// Created by hoshino on 2021/3/27.
//
/*
 * armor_w 甲板的宽度  cx代码里为 config.armor_W
 * armor_h 甲板的宽度  cx代码里为 config.armor_H
 * mtx  相机内参矩阵 ist
 * OpenCV提供solvePnP函数来求解出已知物体相对于头的三维空间坐标系的旋转和平移向量。
 * 平移向量得到的坐标是以摄像头中心为原点的，要得到正确的转角需要对这个坐标进行平移
 */
#include "../include/3Dpnp.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <glog/logging.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <opencv2/core/eigen.hpp>
cv::Point2d Radarpnp::radar_rotate(cv::Mat &Tvec,rm::config::camera& camera_) const {
    cv::Point2d ans;
    Eigen::Vector3d euler_angle( (camera_.pitch-90.0)/ 180 * CV_PI, 0, (double)camera_.yaw/ 180 * CV_PI);
    // 使用Eigen库将欧拉角转换为旋转矩阵
    Eigen::Matrix3d rotation_matrix1;
    rotation_matrix1 = Eigen::AngleAxisd(euler_angle[2], Eigen::Vector3d::UnitZ()) *
                       Eigen::AngleAxisd(euler_angle[1], Eigen::Vector3d::UnitY()) *
                       Eigen::AngleAxisd(euler_angle[0], Eigen::Vector3d::UnitX());
    Eigen::Matrix<double, 3, 1> T_n;
    cv::cv2eigen(Tvec, T_n);
    Eigen::Vector3d P_OC;
    P_OC = rotation_matrix1*T_n;
    LOG(INFO) <<"the armor place" <<P_OC ;
    ans.y = (P_OC(0)/1000+ camera_.coordinate_x) / this->arena_w  * this->picture_h;//宽度
    ans.x = (P_OC(1)/1000 + camera_.coordinate_y) / this->arena_l * this->picture_w;//长度
    return ans;
}

//bool Radarpnp::coordinate_judge(cv::Mat &Tvec, double angle_x, double angle_y, double angle_z) const {
//    if (abs(Tvec.at<double>(0) - camera_.coordinate_x) > 200) {
//        return false;
//    }
//    if (abs(Tvec.at<double>(1) - camera_.coordinate_y) > 300) {
//        return false;
//    }
//    if (abs(Tvec.at<double>(2) - this->radar_coordinate_around.z) > 200) {
//        return false;
//    }
//    if (abs(angle_x - this->radar_angle_around.pitch) > 5) {
//        return false;
//    }
//    if (abs(angle_y - this->radar_angle_around.roll) > 5) {
//        return false;
//    }
//    if (abs(angle_z - this->radar_angle_around.yaw) > 5) {
//        return false;
//    }
//    return true;
//}
//bool Radarpnp::radar_coordinate_init(std::vector<std::vector<cv::Point3f>> &visual_coordinate,
//                                      std::vector<std::vector<cv::Point3f>> &visual_coordinate_camara) {
//    if (visual_coordinate.size() != visual_coordinate_camara.size()) {
//        return false;
//    } else if (visual_coordinate.empty() || visual_coordinate_camara.empty()) {
//        return false;
//    }
//    std::vector<Radar_angle> radar_angle_init;
//    std::vector<cv::Point3d> radar_coor_init;
//    cv::Mat Rvec, Tvec, rotM, invrotM, P;
//    for (int i = 0; i < visual_coordinate.size(); ++i) {
//
//        //PnP解算
//        cv::solvePnP(visual_coordinate[i], visual_coordinate_camara[i], mtx, dist, Rvec, Tvec, false,
//                     cv::SOLVEPNP_ITERATIVE);
//        //旋转向量转换为旋转矩阵
//        cv::Rodrigues(Rvec, rotM);
//        double theta_x = atan2(rotM.at<double>(2, 1), rotM.at<double>(2, 2));//绕x轴俯仰角
//        //绕y轴
//        double theta_y = atan2(-rotM.at<double>(2, 0),
//                               sqrt(rotM.at<double>(2, 1) * rotM.at<double>(2, 1) +
//                                    rotM.at<double>(2, 2) * rotM.at<double>(2, 2)));
//        double theta_z = atan2(rotM.at<double>(1, 0), rotM.at<double>(0, 0));//绕z轴
//        theta_x = theta_x * (180 / CV_PI);
//        theta_y = theta_y * (180 / CV_PI);
//        theta_z = theta_z * (180 / CV_PI);
//        LOG(INFO) << "theta_x: " << theta_x << "theta_y: " << theta_y << "theta_z: " << theta_z << std::endl;
//        printf("theta_x:%.1f  y:%.1f  z:%.1f\n", theta_x, theta_y, theta_z);
//        if (this->coordinate_judge(Tvec, theta_x, theta_y, theta_z)) {
//            Radar_angle radarAngle;
//            radarAngle.pitch = theta_x;
//            radarAngle.roll = theta_y;
//            radarAngle.yaw = theta_z;
//            radar_angle_init.push_back(radarAngle);
//
//            cv::Point3d radar_coor = {Tvec.at<double>(0), Tvec.at<double>(1), Tvec.at<double>(2)};
//            radar_coor_init.push_back(radar_coor);
//            this->status = true;//初始化成功
//        }
//    }
//    if (this->status) {
//        for (int i = 0; i < radar_angle_init.size(); ++i) {
//            camera_.coordinate_x += radar_coor_init[i].x;
//            camera_.coordinate_y += radar_coor_init[i].y;
//            this->radar_coordinate.z += radar_coor_init[i].z;
//
//            this->radar_angle.yaw += radar_angle_init[i].yaw;
//            this->radar_angle.pitch += radar_angle_init[i].pitch;
//            this->radar_angle.roll += radar_angle_init[i].roll;
//        }
//        this->radar_angle.roll += radar_angle_around.roll;
//        this->radar_angle.yaw += this->radar_angle_around.yaw;
//        this->radar_angle.pitch += this->radar_angle_around.pitch;
//        camera_.coordinate_x += camera_.coordinate_x;
//        camera_.coordinate_y += camera_.coordinate_y;
//        this->radar_coordinate.z += this->radar_coordinate_around.z;
//
//        this->radar_angle.roll = radar_angle_around.roll / (radar_angle_init.size() + 1.0);
//        this->radar_angle.yaw = this->radar_angle_around.yaw / (radar_angle_init.size() + 1.0);
//        this->radar_angle.pitch = this->radar_angle_around.pitch / (radar_angle_init.size() + 1.0);
//        camera_.coordinate_x = camera_.coordinate_x / (radar_angle_init.size() + 1.0);
//        camera_.coordinate_y = camera_.coordinate_y / (radar_angle_init.size() + 1.0);
//        this->radar_coordinate.z = this->radar_coordinate_around.z / (radar_angle_init.size() + 1.0);
//
//    }
//    return true;
//}

cv::Point2d Radarpnp::radar_solvePnp(cv::Rect2d &aim_coordinate_camara,int id) const {
    static double x = 170 / 2.0 ;
    static double y = 125 / 2.0 ;
    rm::config::camera  camera_= this->camera_1;
    if(id==1)
    {
      camera_ = this->camera_1;
    }
    else{
      camera_ = this->camera_2;
    }

    //装甲板世界坐标，从开始逆时针 4 点。
    static std::vector<cv::Point3f> world_points = {cv::Point3f(-x, -y, 0),
                                                    cv::Point3f(x, -y, 0),
                                                    cv::Point3f(x, y, 0),
                                                    cv::Point3f(-x, y, 0)};
    cv::Mat Rvec, Tvec, rotM, invrotM, P;
    cv::Mat tuax, ruax;
    std::vector<cv::Point2d> aim = {cv::Point2d(aim_coordinate_camara.x,aim_coordinate_camara.y),
                                    cv::Point2d(aim_coordinate_camara.x+aim_coordinate_camara.width,aim_coordinate_camara.y),
                                    cv::Point2d(aim_coordinate_camara.x+aim_coordinate_camara.width,aim_coordinate_camara.y+aim_coordinate_camara.height),
                                    cv::Point2d(aim_coordinate_camara.x,aim_coordinate_camara.y+aim_coordinate_camara.height)};

    //PnP解算
    cv::solvePnP(world_points, aim, camera_.mtx, camera_.dist, Rvec, Tvec, false, cv::SOLVEPNP_ITERATIVE);
    cv::Point2d ans_rotate = radar_rotate(Tvec,camera_);
    return ans_rotate;
}
 // RADAR_COMP_PNP
