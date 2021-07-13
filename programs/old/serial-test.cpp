//
// Created by Chiro on 2021/1/27.
//

#include <radar.hpp>
#include <rmcamera.h>
#include <utils.h>
#include <minimap.h>
#include <mapping.h>
#include <rmserial.h>
#include <mcu_send.h>

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  if (argc == 1) google::InitGoogleLogging(*argv);

  rmtime.init();
  config.init_from_file();
  mapping_data.load_from_file();

  LOG(INFO) << "Initing serial on " << config.serial_port << "...";
  try {
    rmSerial.init();
  } catch (serial::IOException e) {
    LOG(FATAL) << "Could not open serial port " << config.serial_port << ", exception: " << e.what();
  }
  if (!rmSerial.isConnected()) LOG(FATAL) << "Unable to open serial!";
  LOG(INFO) << "Serial inited ok.";
  LOG(INFO) << "Initing mcu_send...";
  mcuDataSend.init(0);    // red
  minimap.team_us = rm::minimap::team_type::red;
  uint16_t robot = minimap.robot_get_id(rm::minimap::team_type::red, rm::minimap::object_type::hero);
  minimap.robot_add(robot, rm::minimap::object(rm::minimap::object_type::hero,
                                               rm::minimap::team_type::red, ""));
  minimap.robot_update(rm::minimap::map::update_operate::position, robot, std::vector<int>({128, 128}));
  auto data = minimap.get_shapes();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  mcuDataSend.draw(data);
  std::this_thread::sleep_for(std::chrono::seconds(2000));
  return 0;
}