#ifndef RADAR_RM_REFERENCE
#define RADAR_RM_REFERENCE

#include <glog/logging.h>
#include <rmserial.h>
#include <minimap.h>
#include <rmrefs.h>
#include <CRC.h>


namespace rm {

class RmReference {
public:
  uint8_t seq = 0;
  team_type us = team_type::red;
  const std::vector<object_type> target_objs{
          object_type::hero, object_type::engineer, object_type::infantry_3, object_type::infantry_4,
          // object_type::infantry_5,
          object_type::aerial
  };
  std::vector<uint16_t> targets;

  void init_targets() {
    for (const auto &o : target_objs)
      targets.emplace_back(robot_get_id(us, o));
  }

  explicit RmReference(team_type us_) : us(us_) {
    init_targets();
  }

  uint8_t get_next_seq() {
    seq++;
    if (seq == 0xFF) seq = 0;
    return seq;
  }

  ext_student_interactive_header_data_t create_data_header(uint16_t data_cmd_id,
                                                           uint16_t receiver_ID) const {
    // const uint16_t sender_ID = rm::robot_get_id(this->us, rm::object_type::radar);
    const uint16_t sender_ID = 0;
    return ext_student_interactive_header_data_t{.data_cmd_id = data_cmd_id,
            .sender_ID = sender_ID,
            .receiver_ID = receiver_ID};
  }

  template<size_t size>
  void frame_create_header(frame_data_t<size> &frame) {
    // LOG(INFO) << "creating frame header<" << size << ">";
    frame.header.SOF = 0xA5;
    frame.header.data_length = (uint16_t) (size + 6);
    frame.header.seq = get_next_seq();
    frame.header.cmd_id = 0x0301;
    uint8_t crc = Get_CRC8_Check_Sum((uint8_t *) &frame.header,
                                     sizeof(frame.header) - sizeof(frame.header.cmd_id) - 1, CRC8_INIT);
    frame.header.CRC8 = crc;
    // printf("CRC16: 0x%02X\n", crc);
  }

//  template<size_t size>
//  void frame_wrap(frame_data_t<size> &frame, uint8_t data[size]) {
//    memcpy(&frame.data, data, sizeof(data));
//    frame_crc(frame);
//  }

  template<size_t size>
  void frame_crc(frame_data_t<size> &frame) {
    uint16_t crc = Get_CRC16_Check_Sum((uint8_t *) &frame, sizeof(frame) - 2, CRC_INIT);
    frame.crc = crc;
    // printf("CRC8 : 0x%04X\n", crc);
  }

  template<size_t branch>
  frame_data_t<sizeof(graphic_data_struct_t) * branch>
  get_frame_from_graphics(std::vector<graphic_data_struct_t> &graphics_slice, uint16_t receiver) {
    frame_data_t<sizeof(graphic_data_struct_t) * branch> frame;
    frame_create_header(frame);
    constexpr uint16_t data_cmd_id =
            branch == 1 ? 0x0101 : branch == 2 ? 0x0102 : branch == 5 ? 0x0103 : branch == 7 ? 0x0104 : 0x0110;
    auto data_header = create_data_header(data_cmd_id, receiver);
    auto const branch_size = sizeof(graphic_data_struct_t);
    size_t offset = 0;
    // 写入数据头
    memcpy(frame.data + offset, &data_header, sizeof(data_header));
    offset += sizeof(data_header);
    for (const auto &g : graphics_slice) {
      memcpy(frame.data + offset, &g, branch_size);
      offset += branch_size;
    }
    return frame;
  }

  template<size_t branch>
  frame_data_t<sizeof(graphic_data_struct_t) * branch>
  get_frame_branch(std::vector<graphic_data_struct_t> &graphics, size_t offset, uint16_t receiver) {
    auto graphics_slice = std::vector<graphic_data_struct_t>(graphics.begin() + offset, graphics.begin() + branch);
    auto frame = get_frame_from_graphics<branch>(graphics_slice, receiver);
    frame_crc(frame);
    return frame;
  }

  frame_data_t<sizeof(graphic_data_struct_t) + 30>
  get_text_frame(const graphic_data_struct_t &g, uint16_t receiver) {
    frame_data_t<sizeof(graphic_data_struct_t) + 30> frame;
    frame_create_header(frame);
    constexpr uint16_t data_cmd_id = 0x0110;
    auto data_header = create_data_header(data_cmd_id, receiver);
    auto const branch_size = sizeof(graphic_data_struct_t);
    size_t offset = 0;
    // 写入数据头
    memcpy(frame.data + offset, &data_header, sizeof(data_header));
    offset += sizeof(data_header);
    // 写入图像数据
    memcpy(frame.data + offset, &g, branch_size);
    offset += branch_size;
    // 写入文字数据
    std::string text = "U";
    uint8_t text_data[30]{};
    if (text_extra.find(std::string((char *) g.graphic_name)) != text_extra.end())
      text = text_extra[std::string((char *) g.graphic_name)];
    strcpy((char *) text_data, text.c_str());
    memcpy(frame.data + offset, text_data, 30);
    // LOG(INFO) << "Sent text " << text;
    return frame;
  }

  void draw_text(std::vector<graphic_data_struct_t> &graphics, uint16_t target) {
    for (const graphic_data_struct_t &g : graphics) {
      auto frame = get_text_frame(g, target);
      rmSerial->send_data((uint8_t *) &frame, sizeof(frame));
    }
  }

  void draw_graphics(std::vector<graphic_data_struct_t> &graphics, uint16_t target) {
    // 7个图形一轮，剩下发1, 2, 5
    int count = 0;
    while (graphics.size() >= 7 && count <= graphics.size() - 7) {
      auto frame = get_frame_branch<7>(graphics, count, target);
      rmSerial->send_data((uint8_t *) &frame, sizeof(frame));
      count += 7;
    }
    frame_data_t<sizeof(graphic_data_struct_t)> frame1;
    frame_data_t<sizeof(graphic_data_struct_t) * 2> frame2;
    frame_data_t<sizeof(graphic_data_struct_t) * 5> frame5;
    switch (graphics.size() - count) {
      case 0:
        break;
      case 1:
        frame1 = get_frame_branch<1>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame1, sizeof(frame1));
        break;
      case 2:
        frame2 = get_frame_branch<2>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame2, sizeof(frame2));
        break;
      case 3:
        frame1 = get_frame_branch<1>(graphics, count++, target);
        rmSerial->send_data((uint8_t *) &frame1, sizeof(frame1));
        frame2 = get_frame_branch<2>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame2, sizeof(frame2));
        break;
      case 4:
        frame2 = get_frame_branch<2>(graphics, count += 2, target);
        rmSerial->send_data((uint8_t *) &frame2, sizeof(frame2));
        frame2 = get_frame_branch<2>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame2, sizeof(frame2));
        break;
      case 5:
        frame5 = get_frame_branch<5>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame5, sizeof(frame5));
        break;
      case 6:
        frame1 = get_frame_branch<1>(graphics, count++, target);
        rmSerial->send_data((uint8_t *) &frame1, sizeof(frame1));
        frame5 = get_frame_branch<5>(graphics, count, target);
        rmSerial->send_data((uint8_t *) &frame5, sizeof(frame5));
      default:
        break;
    }
  }

  void draw(std::vector<graphic_data_struct_t> &graphics) {
    std::vector<graphic_data_struct_t> images, texts;
    for (const auto &g : graphics)
      if (static_cast<const uint32_t>(minimap::shape::shape_type::text) == g.graphic_type)
        texts.emplace_back(g);
      else images.emplace_back(g);
    LOG(INFO) << "Drawing " << images.size() << " image(s), " << texts.size() << " text(s).";
    for (uint16_t target : targets) {
      draw_graphics(images, target);
      draw_text(texts, target);
    }
  }
};

}

extern rm::RmReference *rmreference;

#endif  // RADAR_RM_REFERENCE