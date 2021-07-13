//
// Created by Chiro on 2021/5/22.
//

#ifndef RADAR_RMREFS_H
#define RADAR_RMREFS_H

#include <cstdint>

template<size_t size>
struct robot_interactive_data_st {
  uint8_t data[size];
};

template<size_t size>
using robot_interactive_data_t = struct robot_interactive_data_st<size>;

typedef struct {
  uint8_t operate_type;
  uint8_t layer;
} ext_client_custom; /*_graphic_delete_t*/

typedef struct {
  uint8_t graphic_name[3];
  uint32_t operate_type: 3;
  uint32_t graphic_type: 3;
  uint32_t layer: 4;
  uint32_t color: 4;
  uint32_t start_angle: 9;
  uint32_t end_angle: 9;
  uint32_t width: 10;
  uint32_t start_x: 11;
  uint32_t start_y: 11;
  uint32_t radius: 10;
  uint32_t end_x: 11;
  uint32_t end_y: 11;
}__attribute__ ((packed)) graphic_data_struct_t;

typedef struct {
  graphic_data_struct_t graphic_data_struct;
} ext_client_custom_graphic_single_t;

typedef struct {
  graphic_data_struct_t graphic_data_struct[2];
} ext_client_custom_graphic_double_t;

typedef struct {
  graphic_data_struct_t graphic_data_struct[5];
} ext_client_custom_graphic_five_t;

typedef struct {
  graphic_data_struct_t graphic_data_struct[7];
} ext_client_custom_graphic_seven_t;

typedef struct {
  graphic_data_struct_t graphic_data_struct;
  uint8_t data[30];
} ext_client_custom_character_t;

typedef struct {
  uint16_t data_cmd_id;
  uint16_t sender_ID;
  uint16_t receiver_ID;
} __attribute__ ((packed)) ext_student_interactive_header_data_t;

typedef struct {
  ext_student_interactive_header_data_t header;
  graphic_data_struct_t body[5];
} __attribute__ ((packed)) radar_data_r;

typedef struct {
  ext_student_interactive_header_data_t header;
  graphic_data_struct_t body;
  uint8_t text[30];
} __attribute__ ((packed)) radar_data_text_r;

typedef struct {
  ext_student_interactive_header_data_t header;
  ext_client_custom body;
} __attribute__ ((packed)) radar_delete_r;

typedef struct {
  uint8_t SOF;
  uint16_t data_length;
  uint8_t seq;
  uint8_t CRC8;
  uint16_t cmd_id;
} __attribute__ ((packed)) frame_header_r;


template<size_t size>
struct frame_data_st {
  frame_header_r header;
  // cmd_id 放在header里面了
  uint8_t data[size + sizeof(ext_student_interactive_header_data_t)];
  uint16_t crc;
} __attribute__ ((packed));

template<size_t size>
using frame_data_t = struct frame_data_st<size>;

#endif //RADAR_RMREFS_H
