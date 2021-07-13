//
// Created by Chiro on 2021/1/23.
//

#ifndef RADAR_RMUTILS_H
#define RADAR_RMUTILS_H

#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>
#include <thread>
#include <map>
#include <cmath>
#include <opencv2/opencv.hpp>


extern std::map<std::string, cv::VideoWriter> rm_utils_video_writers;

namespace rm {

namespace utils {

bool file_exists(const std::string &filename);

//系统时间工具类
class rm_time {
public:
  std::chrono::steady_clock::time_point begin;

  void init() { begin = std::chrono::steady_clock::now(); }

  long long nanoseconds() const {
    auto now = std::chrono::steady_clock::now();
    return (now - begin).count();
  }

  double milliseconds() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(now - begin).count();
  }

  double seconds() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - begin).count();
  }

  static void sleep(double milli) {
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(milli));
  }
};

double getPointLength(const cv::Point2f &p);

cv::VideoWriter initVideoWriter(const std::string &filename_prefix);

cv::VideoWriter getVideoWriter(const std::string &prefix);

void saveVideos(cv::Mat &img, const std::string &prefix);

}   // namespace utils

}   // namespace rm

extern rm::utils::rm_time *rmtime;

#endif //RADAR_RMUTILS_H
