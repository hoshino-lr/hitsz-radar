/*
 * @desp: 雷达站主程序
 * */
#include <radar.hpp>
#include <rmcamera.h>
#include <rmutils.h>
#include <minimap.h>
#include <mapping.h>


bool is_debugging = true;
cv::Mat frame;

// 兼容用变量
// 从代号到ID的转换
std::map<int, uint16_t> ids_us, ids_enemy;

std::vector<rm::minimap::Vec2i> positions_us(6);
std::vector<rm::minimap::Vec2i> positions_enemy(6);

void init() {
  config = new rm::config::general();
  rmtime = new rm::utils::rm_time();
  mapping_data = new rm::config::mapping();
  mapping = new Mapping();
  mapping2 = new MappingTriangle();
  minimap = new rm::minimap::map();

  LOG(INFO) << "Init() started...";
  rmtime->init();
  LOG(INFO) << "Init: rmtime ok.";
  config->init_from_file();
  LOG(INFO) << "Init: config ok.";
  // 初始化小地图组件
  minimap->team_us = rm::team_type::red;
  // 添加自己队伍成员、对方队伍成员
//  // TODO: 用config文件读取相关信息
//  for (int i = 1; i <= 6; i++) {
//    auto type_now = (rm::minimap::object_type) i;
//    uint16_t id = minimap->robot_get_id(rm::minimap::team_type::us, type_now);
//    minimap->robot_add(id, rm::minimap::object(type_now, rm::minimap::team_type::us,
//                                               minimap->object_type_string[type_now]));
//    ids_us[i] = id;
//    id = minimap->robot_get_id(rm::minimap::team_type::enemy, type_now);
//    minimap->robot_add(id, rm::minimap::object(type_now, rm::minimap::team_type::enemy,
//                                               minimap->object_type_string[type_now]));
//    ids_enemy[i] = id;
//  }
  LOG(INFO) << "Init: minimap done.";
  // 初始化KD树数据，从文件读取
  if (mapping_data->load_from_file()) LOG(INFO) << "Init: read mapping file done.";
  else
    LOG(ERROR) << "Init: read mapping file ERROR!";
  // 初始化Radar，填充数据然后建树
  mapping->camera_position = mapping_data->get_mapping_vectors_from();
  mapping->minimap_position = mapping_data->get_mapping_vectors_to();
  mapping->init();
  LOG(INFO) << "Init: radar building ok.";
  // 初始化相机
  rm::camera::open(&config->camConfig, config->camera_exposure, config->camera_gain);
  if (!rm::camera::is_open()) LOG(FATAL) << "Can not open camera!";
  while (!frame.size[0]) rm::camera::get_frame(frame);
  LOG(INFO) << "Init: camera ok.";
}

bool running = false;

void loop() {
  running = true;
  std::default_random_engine rand_e;
  // 随机抖动
  std::uniform_int_distribution<int> rand_u_delta(-10, 10);
  std::vector<RoboInputinfo> input;
  while (running) {
    rm::camera::get_frame(frame);
    if (!frame.size[0]) continue;
//    cv::cvtColor(frame, frame, cv::COLOR_RGB2BGR);

    cv::imshow("frame-src", frame);
    cv::waitKey(1);
    mapping->updateRobotInfo(input);
    mapping->calculateAndPredict();
    // 计算过后的数据更新
    for (int i = 0; i < ROBOTAMOUNT; i++) {
      minimap->robot_update(rm::minimap::map::update_operate::position, ids_enemy[i + 1],
                            cv::Vec2i{mapping->robot[i].current_x, mapping->robot[i].current_y});
    }
    minimap->draw(1);
  }
}

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);
  LOG(INFO) << "Welcome to radar!";

  init();
  loop();

  LOG(INFO) << "Done!";
  return 0;
}