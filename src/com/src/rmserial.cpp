#include <radar.hpp>
#include <rmserial.h>
#include "mcu_send.h"
#include <rmconfig.h>

// receive data
std::mutex receive_mtx;
EnemyConfig receive_config_data;
Mcu_Data_Receive mcu_Data_Receive;

RmSerial::~RmSerial() { stop_thread(); }

bool RmSerial::send_data(uint8_t *data, size_t size) {
#ifndef RADAR_USE_FAKE_TTL
  LOG(INFO) << "writing " << size << " byte(s) to serial";
  return active_port->write(data, size) == size;
#else
  LOG(INFO) << "writing " << size << " byte(s) to virtual serial";
  return true;
#endif
}

void proccess_data(uint8_t *s, uint8_t *e) {
  if (e - s != sizeof(EnemyData)) {
    LOG(ERROR) << "Invalid EnemyData (Invalid Size:" << e - s << ")";
    return;
  }
  EnemyData enemy_data;
  memcpy(&enemy_data, s, sizeof(EnemyData));
  if (enemy_data.start_flag != 's') {
    LOG(ERROR) << "Invalid Enemy Data (Invalid Data)";
    return;
  }
  receive_mtx.lock();
  mcu_Data_Receive.data_update(&enemy_data);

  receive_mtx.unlock();
}


void receive_data(RmSerial *rm_serial) {
#ifndef RADAR_USE_FAKE_TTL
  LOG(INFO) << "recieve thread started!";
  static uint8_t buff[100];
  uint8_t *buffer_tail = buff;
  serial::Serial *port = rm_serial->active_port;
  while (rm_serial->thread_running) {
    size_t wait_in_buffer = port->available();
    if (wait_in_buffer) {
      port->read(buffer_tail, wait_in_buffer);
      buffer_tail += wait_in_buffer;
      // LOG(INFO) << "buffer_tail: " << buffer_tail;
      LOG(INFO) << "buf size " << buffer_tail - buff;
//      if (buffer_tail[-1] == 'e') {
//        *buffer_tail = 0;
//        proccess_data(buff, buffer_tail);
//        buffer_tail = buff;
//      }
    }
  }
#else
  LOG(INFO) << "recieve thread should not started!";
#endif
}

void RmSerial::start_thread() {
  if (init_success) {
    thread_running = true;
    std::thread task(receive_data, this);
    task.detach();
  }
}

void RmSerial::stop_thread() {
  if (init_success) {
    thread_running = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

bool RmSerial::init() {
#ifndef RADAR_USE_FAKE_TTL
  LOG(INFO) << "Serial Send Size:" << sizeof(EnemyConfig);
  LOG(INFO) << "Serial Recieve Size:" << sizeof(EnemyData);
  active_port = new serial::Serial(config->serial_port, 115200,
                                   serial::Timeout::simpleTimeout(1000));
  init_success = true;
  //初始化数据接受结构体
  receive_config_data.pridict_x = -1;
  receive_config_data.pridict_y = -1;
  receive_config_data.type = -1;
  //开启数据接受线程
//  start_thread();
  if (active_port->isOpen()) {
    LOG(INFO) << "Successfully initialized port " << config->serial_port;
    return true;
  } else {
    LOG(ERROR) << "failed to initialize port " << config->serial_port;
    return false;
  }
#else
  LOG(INFO) << "Serial Using Virtual serial...";
  init_success = true;
  return true;
#endif
}

// 串口（未初始化）
RmSerial *rmSerial;