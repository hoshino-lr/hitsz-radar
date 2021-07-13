//
// Created by hoshino on 2021/1/25.
//

#include "mcu_send.h"
#include "CRC.h"

using namespace cv;
using namespace std;

ext_client_IDs ext_client_IDS;
ext_client_cmds ext_client_cmdS;

uint16_t receiver_red_IDs[5] = {ext_client_IDS.hero_red, ext_client_IDS.eng_red, ext_client_IDS.sol_red1,
                                ext_client_IDS.sol_red2, ext_client_IDS.sol_red3};
uint16_t receiver_blue_IDs[5] = {ext_client_IDS.hero_blue, ext_client_IDS.eng_blue, ext_client_IDS.sol_blue1,
                                 ext_client_IDS.sol_blue2, ext_client_IDS.sol_blue3};
#if 0
void draw_mcu(graphic_data_struct_t *data, int main_color, int operation, uint8_t *draw_text[30]) {
    radar_data radar_data;
    ext_student_interactive_header_data_t header_data;
    graphic_data_struct_t *send_data = data;
    switch (operation) {
        case 1://draw_
            header_data.data_cmd_id = ext_client_cmdS.draw_;
            break;
        case 2://modify
            header_data.data_cmd_id = ext_client_cmdS.draw_;
            break;
        case 3://delete_
            header_data.data_cmd_id = ext_client_cmdS.delete_;
            break;
    }
    if (data->graphic_type != 6) {
        for (int i = 0; i < 5; ++i) {
            if (main_color == 0)//red
            {
                header_data.sender_ID = ext_client_IDS.radar_red;
                header_data.receiver_ID = receiver_red_IDs[i];
            } else//blue
            {
                header_data.sender_ID = ext_client_IDS.radar_blue;
                header_data.receiver_ID = receiver_blue_IDs[i];
            }
            do {
                radar_data.header = &header_data;
                radar_data.body = data;
                rmSerial.send_data((uint8_t *) (&radar_data), sizeof(radar_data));
            } while (!rmSerial.isConnected());
        }
    } else {
        for (int i = 0; i < 5; ++i) {
            if (main_color == 0)//red
            {
                header_data.sender_ID = ext_client_IDS.radar_red;
                header_data.receiver_ID = receiver_red_IDs[i];
            } else//blue
            {
                header_data.sender_ID = ext_client_IDS.radar_blue;
                header_data.receiver_ID = receiver_blue_IDs[i];
            }
            do {
                radar_data.header = &header_data;
                radar_data.body = data;
                rmSerial.send_data((uint8_t *) (&radar_data), sizeof(radar_data));
            } while (!rmSerial.isConnected());
        }
    }
}

#else

// size must be less than 5
void Mcu_Data_Send::draw_mcu(vector<rm::minimap::shape> &draw_data) {
  int operation;
  operation = (int) draw_data[0].operate_type;
  switch (operation) {
    case 1://draw_
      radar_data.header.data_cmd_id = ext_client_cmdS.draw_;
      break;
    case 2://modify
      radar_data.header.data_cmd_id = ext_client_cmdS.draw_;
      break;
    case 3://delete_
    {
      radar_delete.header = radar_data.header;
      radar_delete.body.layer = draw_data[0].layer;
      radar_delete.body.operate_type = (uint8_t) draw_data[0].operate_type;
    }
      return;
  }
  for (int i = 0; i < draw_data.size(); i++) {
    {
      radar_data.body[i] = draw_data[i].pack();
    }
  }
}

void Mcu_Data_Send::draw_mcu_text(vector<rm::minimap::shape> &draw_data) {
  for (auto &i : draw_data) {
    {
      radar_data_text.body = i.pack();
      radar_data_text.header.data_cmd_id = ext_client_cmdS.draw_text;
      // radar_data_text.text = draw_data[i].name.c_str();
      // 字符串复制，获取文字
      strcpy((char *) radar_data_text.text, rm::minimap::map::text_extra_get(i.name).c_str());
    }

  }
}

void Mcu_Data_Send::send_data() {
  if (!rmSerial->isConnected()) {
    LOG(WARNING) << "serial didn't connected" << std::endl;
  } else {
    dump_data_tmp();
    LOG(INFO) << "sending " << dat.size() << " bytes";
    rmSerial->send_data(data_tmp, dat.size());
  }
}

bool Mcu_Data_Send::init(int color) {
  bool i;
  while (!rmSerial->init_success) {
    i = rmSerial->init();
  }
  seq = 1;
  main_color = color;
  return i;
}

bool Mcu_Data_Send::isConnected() {
  return rmSerial->isConnected();
}

void Mcu_Data_Send::generate_data_bytes(uint16_t id_sender, uint16_t id_receiver) {
  radar_data_text.header.sender_ID = id_sender;
  radar_data_text.header.receiver_ID = id_receiver;

  data_cat((uint8_t *) &frame_header, sizeof(frame_header));
  data_cat((uint8_t *) &radar_delete, sizeof(radar_delete));
  data_cat((uint8_t *) &frame_tail, sizeof(frame_tail));

  Append_CRC8_Check_Sum((unsigned char *) &frame_header, sizeof(frame_header) - 2);
  dump_data_tmp();
  uint16_t crc16 = Get_CRC16_Check_Sum(data_tmp, dat.size(), CRC_INIT);
  dat.emplace_back((uint8_t) (crc16 & 0xffu));
  dat.emplace_back((uint8_t) ((crc16 >> 8) & 0xffu));
}

void Mcu_Data_Send::generate_data(uint16_t id_sender, uint16_t id_receiver) {
  frame_header.SOF = 0xA5;
  frame_header.data_length = sizeof(radar_data);
  frame_header.seq = seq;
  seq++;
  frame_header.cmd_id = 0x0301;

  generate_data_bytes(id_sender, id_receiver);
}

void Mcu_Data_Send::generate_data_delete(uint16_t id_sender, uint16_t id_receiver) {
  frame_header.SOF = 0xA5;
  frame_header.data_length = sizeof(radar_delete_r);
  frame_header.seq = seq;
  seq++;
  frame_header.cmd_id = 0x0301;

  generate_data_bytes(id_sender, id_receiver);
}

void Mcu_Data_Send::generate_data_text(uint16_t id_sender, uint16_t id_receiver) {
  //
  frame_header.SOF = 0xA5;
  frame_header.data_length = sizeof(radar_data_text_r);
  frame_header.seq = seq;
  seq++;
  frame_header.cmd_id = 0x0301;

  generate_data_bytes(id_sender, id_receiver);
}

void Mcu_Data_Send::draw(const std::vector<rm::minimap::shape> &draw_data) {
  std::vector<uint16_t> receivers{
          rm::robot_get_id(rm::team_type::red, rm::object_type::hero),
          rm::robot_get_id(rm::team_type::red, rm::object_type::engineer),
          rm::robot_get_id(rm::team_type::red, rm::object_type::infantry_3),
          rm::robot_get_id(rm::team_type::red, rm::object_type::infantry_4),
          rm::robot_get_id(rm::team_type::red, rm::object_type::aerial),
  };
  uint16_t sender = rm::robot_get_id(rm::team_type::red, rm::object_type::radar);
  for (uint16_t receiver : receivers) {
    dat.clear();
    // 区分文字和普通的图形
    std::vector<rm::minimap::shape> shapes_text, shapes;
    for (const auto &s : draw_data)
      if (s.graphic_type == rm::minimap::shape::shape_type::text)
        shapes_text.emplace_back(s);
      else
        shapes.emplace_back(s);
    // 绘图，每5个图形一组
    for (int i = 0; i < shapes.size(); i += 5) {
      auto start = shapes.begin() + i;
      auto stop = shapes.begin() + i + (i + 5 < shapes.size() ? 5 : shapes.size() - i);
      std::vector<rm::minimap::shape> cut(start, stop);
      draw_mcu(cut);
      generate_data(sender, receiver);
      send_data();
    }
    // 绘制文字，每个文字发送一次
    for (const auto &t : shapes_text) {
      std::vector<rm::minimap::shape> cut({t});
      draw_mcu_text(cut);
      generate_data_text(sender, receiver);
      send_data();
    }
  }
}

bool Mcu_Data_Receive::init() {
  for (int i = 0; i < 5; ++i) {
    data_state[i] = false;
  }
  return true;
}

Mcu_Data_Receive::~Mcu_Data_Receive() {
  for (int i = 0; i < 5; ++i) {
    data_state[i] = false;
  }
}


void Mcu_Data_Receive::data_update(EnemyData *enemydata_input) {
  int robo_num;
  switch (enemydata_input->self_type) {
    case 1:
      data_state[robo_type.hero] = false;
      robo_num = robo_type.hero;
      break;
    case 3:
      data_state[robo_type.sol1] = false;
      robo_num = robo_type.sol1;
      break;
    case 4:
      data_state[robo_type.sol2] = false;
      robo_num = robo_type.sol2;
      break;
    case 5:
      data_state[robo_type.sol3] = false;
      robo_num = robo_type.sol3;
      break;
  }
  enemydata->type = enemydata_input->type;
  enemydata->pridict_x = enemydata_input->distance * cos(enemydata_input->angle) * zoom + enemydata_input->current_x;
  enemydata->pridict_y = enemydata_input->distance * cos(enemydata_input->angle) * zoom + enemydata_input->current_y;
  data_state[robo_num] = true;
}


// 发送图像数据类
Mcu_Data_Send *mcuDataSend;
#endif