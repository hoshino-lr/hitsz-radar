//
// Created by Chiro on 2021/5/24.
//

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

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);
  LOG(INFO) << "Welcome to radar!";

  init();
  std::this_thread::sleep_for(std::chrono::milliseconds(800));

  std::vector<graphic_data_struct_t> graphics;
  graphics.emplace_back(graphic_data_struct_t{});
  graphics.emplace_back(graphic_data_struct_t{});
  frame_data_t<sizeof(graphic_data_struct_t) * 2> frame2 = rmreference->get_frame_branch<2>(graphics, 0, 0);
  printf("sizeof(frame2) = %llu\n", sizeof(frame2));

  auto *data = (uint8_t *) &frame2;
  size_t i;
  for (i = 0; i < sizeof(frame2); i++) {
    printf("%02X ", data[i]);
    if (i % 16 == 15) {
      printf(" | (%02llu - %02llu)\n", i - 15, i);
    }
  }
  printf(" | (%02llu - %02llu)\n", i - 15, i);

  return 0;
}