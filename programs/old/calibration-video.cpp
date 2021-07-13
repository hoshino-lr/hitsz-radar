/*
 * @desp: 标定程序测试
 * 录一段像
 * */
#include <radar.hpp>
#include <rmcamera.h>
#include <utils.h>
#include <minimap.h>
#include <mapping.h>


using namespace std;

cv::Mat frame;

std::string gmt_time_now() {
  auto now = std::chrono::system_clock::now();
  //通过不同精度获取相差的毫秒数
  uint64_t dis_millseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count()
                             - std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;
  time_t tt = std::chrono::system_clock::to_time_t(now);
  auto time_tm = localtime(&tt);
  char strTime[25] = { 0 };
  sprintf(strTime, "%d-%02d-%02d_%02d-%02d-%02d_%03d", time_tm->tm_year + 1900,
          time_tm->tm_mon + 1, time_tm->tm_mday, time_tm->tm_hour,
          time_tm->tm_min, time_tm->tm_sec, (int)dis_millseconds);
  return std::string(strTime);
}

bool running = true;
std::string filename;

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);

  rmtime.init();
  config.init_from_file();
  camera.open(&config.camConfig, config.camera_exposure, config.camera_gain);

  camera.get_frame(frame);

  filename = gmt_time_now() + ".avi";
  LOG(INFO) << "Writing to " << filename;

  cv::VideoWriter out(filename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
                      60.0,cv::Size(frame.size[0], frame.size[1]));

  while (running) {
    camera.get_frame(frame);
    if (frame.size[0] == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(300));
      LOG(WARNING) << "Got empty frame!";
      continue;
    }
    cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);
    cv::imshow("frame", frame);
    out << frame;
    if (cv::waitKey(1) == 27) running = false;
  }

  out.release();

  return 0;
}