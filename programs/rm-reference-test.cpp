#include <radar.hpp>
#include <rmcamera.h>
#include <rmutils.h>
#include <minimap.h>
#include <mapping.h>
#include <rmserial.h>
#include <rmreference.h>


void init() {
  config = new rm::config::general();
  rmtime = new rm::utils::rm_time();
  mapping_data = new rm::config::mapping();
  mapping = new Mapping();
  mapping2 = new MappingTriangle();
  minimap = new rm::minimap::map();
  rmSerial = new RmSerial();
  // mcuDataSend = new Mcu_Data_Send();


  LOG(INFO) << "Init() started...";
  rmtime->init();
  LOG(INFO) << "Init: rmtime ok.";
  config->init_from_file();
  LOG(INFO) << "Init: config ok.";
  // 初始化小地图组件
  minimap->team_us = rm::team_type::red;
  LOG(INFO) << "Init: minimap done.";

  LOG(INFO) << "Initing serial on " << config->serial_port << "...";
  try {
    rmSerial->init();
  } catch (serial::IOException &e) {
    LOG(FATAL) << "Could not open serial port " << config->serial_port << ", exception: " << e.what();
  }
  if (!rmSerial->isConnected()) LOG(FATAL) << "Unable to open serial!";
  LOG(INFO) << "Serial inited ok.";
  LOG(INFO) << "Initing mcu_send...";
  // mcuDataSend->init(0);    // red
  rmreference = new rm::RmReference(rm::team_type::red);
  LOG(INFO) << "Init: radar building ok.";
}

#define LOG_SIZE(x) LOG(INFO) << "sizeof " #x " = " << sizeof(x)

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);
  LOG(INFO) << "Welcome to radar!";

  init();

  // 查看大小
  LOG_SIZE(ext_student_interactive_header_data_t);
  LOG_SIZE(graphic_data_struct_t);
  LOG_SIZE(radar_data_r);
  LOG_SIZE(radar_data_text_r);
  LOG_SIZE(radar_delete_r);
  LOG_SIZE(frame_header_r);
  LOG_SIZE(frame_data_t<sizeof(graphic_data_struct_t) * 1>);
  LOG_SIZE(frame_data_t<sizeof(graphic_data_struct_t) * 2>);
  LOG_SIZE(frame_data_t<sizeof(graphic_data_struct_t) * 5>);
  LOG_SIZE(frame_data_t<sizeof(graphic_data_struct_t) * 7>);

  LOG_SIZE(frame_header_r);
  LOG(INFO) << "DATA " << sizeof(graphic_data_struct_t) * 2 + sizeof(ext_student_interactive_header_data_t);
  LOG(INFO) << "DATA(1) " << sizeof(graphic_data_struct_t) + sizeof(ext_student_interactive_header_data_t);
  LOG_SIZE(uint16_t);
  LOG_SIZE(frame_data_t<sizeof(graphic_data_struct_t) * 2>);

  minimap->team_us = rm::team_type::red;
  uint16_t robot = minimap->map_robot_get_id(rm::team_type::red, rm::object_type::hero);
  auto obj = rm::minimap::object(rm::object_type::hero,
                                 rm::team_type::red,
                                 minimap->object_type_string[(rm::object_type) robot]);
  minimap->robot_add(robot, obj);
  //  std::vector<int> vec{128, 128};
  while (rand() % 128 < 256) {
    rm::minimap::Vec2i vec{rand() % 128, rand() % 128};
    minimap->robot_update(rm::minimap::map::update_operate::position, robot, vec);
    auto data = minimap->get_shapes();
    // mcuDataSend->draw_graphics(data);
     auto data_t = minimap->get();
    rmreference->draw(data_t);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));

  LOG(INFO) << "Done!";
  return 0;
}