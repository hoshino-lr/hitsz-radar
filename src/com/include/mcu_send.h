//
// Created by hoshino on 2021/1/25.
//

#ifndef MCU_SEND_H
#define MCU_SEND_H

#include <radar.hpp>
#include <minimap.h>
#include <rmserial.h>
#include <rmrefs.h>

typedef struct ext_client_cmds_s {
  uint16_t draw_ = 0x0105;
  uint16_t delete_ = 0x0100;
  uint16_t draw_text = 0x0110;
} ext_client_cmds;

typedef struct ext_client_IDs_s {
  uint16_t hero_red = 0x0101;
  uint16_t hero_blue = 0x0111;
  uint16_t eng_red = 0x0102;
  uint16_t eng_blue = 0x0112;
  uint16_t sol_red2 = 0x0104;
  uint16_t sol_blue2 = 0x0114;
  uint16_t sol_red3 = 0x0116;
  uint16_t sol_blue3 = 0x0115;
  uint16_t sol_red1 = 0x0103;
  uint16_t sol_blue1 = 0x0113;
  uint16_t radar_red = 0x0009;
  uint16_t radar_blue = 0x006D;
} ext_client_IDs;

struct Robo_type {
  int hero = 0;
  int sol1 = 1;
  int sol2 = 2;
  int sol3 = 3;
};

class Mcu_Data_Receive {
public:
  // 是代码初始化
  EnemyConfig enemydata[4]{};
  int zoom{};
  bool data_state[4]{};
  Robo_type robo_type;

  bool init();

  ~Mcu_Data_Receive();

  void data_update(EnemyData *enemydata);
};


class Mcu_Data_Send {
public:
  radar_data_r radar_data;
  radar_data_text_r radar_data_text;
  radar_delete_r radar_delete;
//    RmSerial rmSerial;    // 放在全局变量
  frame_header_r frame_header;
  uint16_t frame_tail;
  uint8_t data_tmp[1024 * 5];
  std::vector<uint8_t> dat;
  uint8_t seq;
  int main_color;

  void generate_data_bytes(uint16_t id_sender, uint16_t id_receiver);

  void dump_data_tmp() {
    size_t offset = 0;
    for (const auto &c : dat) data_tmp[offset++] = c;
  }

  void data_cat(uint8_t *dist, size_t size) {
    while (size--) dat.emplace_back(*(dist++));
  }

  void draw_mcu_text(std::vector<rm::minimap::shape> &draw_data);

  void draw_mcu(std::vector<rm::minimap::shape> &draw_data);

  void send_data();

  void generate_data(uint16_t id_sender, uint16_t id_receiver);

  void generate_data_text(uint16_t id_sender, uint16_t id_receiver);

  void generate_data_delete(uint16_t id_sender, uint16_t id_receiver);

  bool init(int color);

  static bool isConnected();

  // 调用这个函数
  void draw(const std::vector<rm::minimap::shape> &draw_data);
};

extern Mcu_Data_Receive mcu_Data_Receive;
extern Mcu_Data_Send *mcuDataSend;       // 放在mcu_send.cpp内
#endif //HITSZ_RM_CV_DRAW_H
