/*
 * @desp: 标定程序
 * 用于高效地对数据进行标定以得到小地图对应数据
 * */
#include <radar.hpp>
#include <rmcamera.h>
#include <utils.h>
#include <minimap.h>

using namespace std;

cv::Mat backgroulnd;
cv::Point map_now;
cv::Point object_now;
std::mutex points_lock;
// 按键步长
const int key_d = 5;
bool running = true;
bool detecting_running = false;

string filename = "calibration2.bin";
std::mutex data_lock;
std::vector<std::pair<std::vector<int>, std::vector<int>>> mapping_data_vec;

// 记录一下颜色，方便判断
std::vector<cv::Scalar> colors;
int color_i = 0;
std::default_random_engine rand_e;
std::uniform_int_distribution<int> rand_color(0, 255);

std::mutex frame_lock;
cv::Mat frame;

void draw_point(cv::Mat &drawing, cv::Point center, const cv::Scalar& color) {
  cv::circle(drawing, center, 3, color, -1);
}
void draw_point(cv::Mat &drawing, std::vector<int> pt, const cv::Scalar& color) {
  cv::Point center(pt[0], pt[1]);
  draw_point(drawing, center, color);
}

// 另外开一个线程来获取摄像头中的物体坐标
void detecting() {
  detecting_running = true;
  while (detecting_running) {
    try {
      if (!(frame_lock.try_lock() && data_lock.try_lock())) throw "locking, retry.";
      camera.get_frame(frame);
      cv::circle(frame, object_now, 10, cv::Scalar(0, 0, 255), -1);
      color_i = 0;
      for (const auto &p : mapping_data_vec)
        draw_point(frame, p.first, colors[color_i++]);
      data_lock.unlock();
      cv::imshow("frame", frame);
      frame_lock.unlock();
      // 不同线程的 waitKey 会出现bug
//      cv::waitKey(1);
    } catch(const char * e) {
      LOG(WARNING) << e;
    } catch (...) {
      LOG(ERROR) << "Detecting meets error!";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

void on_mouse(int event, int x, int y, int flags, void *ustc) {
  if (!(event == cv::EVENT_LBUTTONUP || event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_MOUSEMOVE)) return;
  points_lock.lock();
  object_now = cv::Point(x, y);
  points_lock.unlock();
}

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);

  rmtime.init();
  config.init_from_file();

  camera.open(&config.camConfig, config.camera_exposure, config.camera_gain);
  if (!camera.is_open()) LOG(FATAL) << "Can not open camera!";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  do {
     camera.get_frame(frame);
  } while (frame.size[0] == 0);
  for (int i = 0; i < 10; i++) camera.get_frame(frame);


  if (!rm::utils::file_exists(filename)) {
    mapping_data.write_to_file(filename);
  }
  mapping_data.load_from_file(filename);
  LOG(INFO) << "Got " << mapping_data.get_mapping()->count << " pairs!";
  mapping_data_vec = mapping_data.get_mapping_vectors();
  background = cv::imread("radar.png");
  cv::imshow("minimap", background);
  cv::imshow("frame", frame);

  cv::setMouseCallback("frame", on_mouse);

  future<void> future_detecting = async(detecting);

  // 把已经标定的点绘制出来
  cv::Mat drawing = background.clone();
  for (const auto &p : mapping_data_vec) {
    cv::Scalar color(rand_color(rand_e), rand_color(rand_e), rand_color(rand_e));
    colors.emplace_back(color);
    draw_point(drawing, p.second, color);
  }

  while (running) {
    cv::Mat bg_fork = drawing.clone();
    int key = cv::waitKey(1);
    points_lock.lock();
    // 因为用了引用，还是要一直锁着
    int &x = map_now.x, &y = map_now.y;
    int &m = object_now.x, &n = object_now.y;
    // 空格就保存当前数据到内存
    if (key == 32) {
      LOG(INFO) << "push to data in memory...";
      data_lock.lock();
      mapping_data_vec.emplace_back(pair<vector<int>, vector<int>>(
              {
                      {object_now.x, object_now.y},
                      {map_now.x,    map_now.y}
              }
      ));
      cv::Scalar color = cv::Scalar(rand_color(rand_e), rand_color(rand_e), rand_color(rand_e));
      colors.emplace_back(color);
      draw_point(drawing, map_now, color);
      data_lock.unlock();
    } else if (key == 13) {
      // 回车就写入磁盘
      LOG(INFO) << "writing to disk...";
      data_lock.lock();
      mapping_data.load_from_vector(mapping_data_vec);
      data_lock.unlock();
      mapping_data.write_to_file(filename);
    } else if (key == 'q' || key == 27) {
      detecting_running = false;
      LOG(INFO) << "writing to disk and quit...";
      data_lock.lock();
      mapping_data.load_from_vector(mapping_data_vec);
      data_lock.unlock();
      mapping_data.write_to_file(filename);
      break;
    } else if (key == 'd') {
      x += key_d;
      if (x >= background.size[1]) x -= key_d;
    } else if (key == 'a') {
      x -= key_d;
      if (x <= 0) x += key_d;
    } else if (key == 's') {
      y += key_d;
      if (y >= background.size[0]) y -= key_d;
    } else if (key == 'w') {
      y -= key_d;
      if (y <= 0) y += key_d;
    } else if (key == 'l') {
      m += key_d;
      frame_lock.lock();
      if (m >= frame.size[1]) m -= key_d;
      frame_lock.unlock();
    } else if (key == 'j') {
      m -= key_d;
      if (m <= 0) m += key_d;
    } else if (key == 'k') {
      n += key_d;
      frame_lock.lock();
      if (n >= frame.size[0]) n -= key_d;
      frame_lock.unlock();
    } else if (key == 'i') {
      n -= key_d;
      if (n <= 0) n += key_d;
    }
    points_lock.unlock();
    cv::circle(bg_fork, map_now, 10, cv::Scalar(0, 0, 255), -1);
    cv::imshow("minimap", bg_fork);
  }
  future_detecting.get();
  return 0;
}