/*
 * @desp: 测试映射方式
 * */
#include <radar.hpp>
#include <rmcamera.h>
#include <utils.h>
#include <minimap.h>
#include <mapping.h>

uint16_t id;
uint16_t id2;
std::mutex pos_lock;
rm::minimap::Vec2i pos;
cv::Mat frame;

void on_mouse(int event, int x, int y, int flags, void *ustc) {
  if (!(event == cv::EVENT_LBUTTONUP || event == cv::EVENT_LBUTTONDOWN || event == cv::EVENT_MOUSEMOVE)) return;
  pos_lock.lock();
  pos = rm::minimap::Vec2i(x, y);
  pos_lock.unlock();
}

void init() {
  LOG(INFO) << "Init() started...";
  rmtime.init();
  LOG(INFO) << "Init: rmtime ok.";
  config.init_from_file();
  LOG(INFO) << "Init: config ok.";
//  // 初始化小地图组件
//  minimap.team_us = rm::minimap::team_type::red;
//  // 添加调试：一个对方英雄和一个己方英雄
//  id = minimap.robot_get_id(rm::minimap::team_type::us, rm::minimap::object_type::hero);
//  minimap.robot_add(id, rm::minimap::object(rm::minimap::object_type::hero, rm::minimap::team_type::enemy, "己方英雄"));
//  id2 = minimap.robot_get_id(rm::minimap::team_type::enemy, rm::minimap::object_type::hero);
//  minimap.robot_add(id2, rm::minimap::object(rm::minimap::object_type::hero, rm::minimap::team_type::enemy, "敌方英雄"));
//  LOG(INFO) << "Init: minimap done.";
  // 初始化KD树数据，从文件读取
  if (mapping_data.load_from_file("calibration2.bin"))
    LOG(INFO) << "Init: read mapping file done." << "Read " << mapping_data.mapping_data_m.count << " entries.";
  else
    LOG(ERROR) << "Init: read mapping file ERROR!";
//  // 初始化Mapping，填充数据然后建树
//  mapping.camera_position = mapping_data.get_mapping_vectors_from();
//  mapping.minimap_position = mapping_data.get_mapping_vectors_to();
//  mapping.init();
//  LOG(INFO) << "Init: radar building ok.";
  camera.open(&config.camConfig, config.camera_exposure, config.camera_gain);
  if (!camera.is_open()) LOG(FATAL) << "Can not open camera!";
  LOG(INFO) << "Init: camera ok.";
  while (!frame.size[0]) camera.get_frame(frame);
  // frame 可能不够大...
  mapping2 = MappingTriangle(cv::Size(frame.size[0] * 2, frame.size[1] * 2));
  mapping2.init(mapping_data.get_mapping_vectors());
}

bool running = false;

void loop() {
  running = true;
  std::vector<RoboInputinfo> input;
  while (running) {
    camera.get_frame(frame);
    cv::resize(frame, frame, cv::Size(frame.size[1] * 2, frame.size[0] * 2));
    if (!frame.size[0]) continue;
//    cv::imshow("frame-src", frame);
//    cv::waitKey(1);
//    // 数据转换
//    input.clear();
//    pos_lock.lock();
//    input.emplace_back((RoboInputinfo) {
//            pos.x, pos.y, 1, (int) (rm::minimap::object_type::hero)
//    });
//    pos_lock.unlock();
//    // 数据更新
//    mapping.updateRobotInfo(input);
////    std::cout << "[" << input[0].x << ", " << input[0].y << "]" << std::endl;
//    mapping.calculateAndPredict();
//    // 计算过后的数据更新
//    minimap.robot_update(rm::minimap::map::update_operate::position, id, (std::vector<int>)
//            {
//                    mapping.robot[0].current_x, mapping.robot[0].current_y
//            });
    // 第二方法测试
    pos_lock.lock();
    cv::Point mpt = mapping2.get_mapping_point(cv::Point(pos.x, pos.y));
    pos_lock.unlock();
//    LOG(INFO) << "NOW: " << mpt;
//    minimap.robot_update(rm::minimap::map::update_operate::position, id2, (std::vector<int>) {mpt.x, mpt.y});
//    minimap.draw(1);
//    cv::Mat cal;
//    cv::cvtColor(mapping2.calibration, cal, CV_8UC3);
    cv::Mat cal = mapping2.calibration2.clone();
    cv::Mat c_map = mapping2.calibration_map.clone();
    cv::circle(cal, cv::Point(pos.x, pos.y), 3, cv::Scalar(127, 127, 255), -1);
    cv::circle(c_map, mpt, 3, cv::Scalar(127, 127, 255), -1);
//    cv::imshow("calibration", mapping2.calibration);
    cv::imshow("calibration2", cal);
    cv::imshow("calibration_map", c_map);
    auto key = cv::waitKey(1);
    if (key == 27 || key == 'q') {
      running = false;
    }
//    cv::imshow("calibration2", cal);
//    cv::imshow("calibration3", cal * 3);
  }
}

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);
  LOG(INFO) << "Welcome to radar!";

  init();

//  cv::imshow("frame-src", frame);
  cv::imshow("calibration2", frame);
//  cv::setMouseCallback("frame-src", on_mouse);
  cv::setMouseCallback("calibration2", on_mouse);

  loop();

  LOG(INFO) << "Done!";
  return 0;
}