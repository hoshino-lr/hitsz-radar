//
// Created by Chiro on 2021/3/14.
//

#include <rmutils.h>

//程序运行时时间类
std::map<std::string, cv::VideoWriter> rm_utils_video_writers;

namespace rm {

namespace utils {

bool file_exists(const std::string &filename) {
  // 尝试用读的方式打开文件
  FILE *fp = fopen(filename.c_str(), "r");
  if (!fp) return false;
  fclose(fp);
  return true;
}

double getPointLength(const cv::Point2f &p) {
  return sqrt(p.x * p.x + p.y * p.y);
}

cv::VideoWriter initVideoWriter(const std::string &filename_prefix) {
  cv::VideoWriter video;
  std::string file_name = filename_prefix + ".avi";
  video.open(file_name, cv::VideoWriter::fourcc('P', 'I', 'M', 'I'), 90, cv::Size(640, 480), true);
  return video;
}

cv::VideoWriter getVideoWriter(const std::string &prefix) {
  auto iter = rm_utils_video_writers.find(prefix);
  if (iter != rm_utils_video_writers.end()) return iter->second;
  cv::VideoWriter now = initVideoWriter("/video/" + prefix);
  rm_utils_video_writers[prefix] = now;
  return now;
}

void saveVideos(cv::Mat &img, const std::string &prefix) {
  if (img.empty()) return;
  cv::VideoWriter writer = getVideoWriter(prefix);
  writer.write(img);
}

}

}