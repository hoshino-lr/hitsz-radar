//
// Created by Chiro on 2021/1/23.
//

#pragma once

#ifndef RADAR_RMCONFIG_H
#define RADAR_RMCONFIG_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <cstring>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <glog/logging.h>
#include <json/json.h>
#include <camera/cam_wrapper.h>
#include "rmcamera_config.h"
#include <constants.h>

namespace rm {

namespace config {

class general {
public:
    const char* configPath = "rmconfig.json";
    //config
    //
    int run_mode = 0;
    bool show_origin = false;
    bool show_armor_box = true;
    bool show_light_box = false;
    bool show_class_id = false;
    bool show_net_box = false;
    bool wait_uart = false;
    bool use_pnp = false;
    bool save_video = true;
    bool show_light_blobs = false;
    bool save_labelled_boxes = false;
    bool show_pnp = true;
    bool log_send_target = false;
    bool use_video = false;
    std::string video_path = "video.mp4";
    std::string camera_sn = "NULL";
    std::set<std::string> ignore_types = {"RE","BE"};
    //energy config
    bool show_energy_extra = false;
    bool show_energy = false;
    bool save_mark = false;
    bool show_process = false;
    bool show_info = false;
    //serial
    bool show_mcu_info = false;
    std::string serial_port = "/dev/ttyUSB0";
    //graphic
    bool has_show = false;
    //DATA
    int ARMOR_CAMERA_EXPOSURE = 4000;
    int ARMOR_CAMERA_GAIN = 8;
    int ENERGY_CAMERA_EXPOSURE = 4000;
    int ENERGY_CAMERA_GAIN = 5;
    int ENEMY_COLOR = ENEMY_BLUE;
    int ARMOR_H = 58;
    int ARMOR_W = 230;
    int IMAGE_CENTER_X = 320;
    int IMAGE_CENTER_Y = 320;
    //Camera
    camera camConfig1 = camera(1084.32,1084.05,339.53,231.71,-0.0876,0.32596,-0.08626,-0.002323,0.001002,0,-45,0,5,0,"NULL");
    camera camConfig2 = camera(1084.32,1084.05,339.53,231.71,-0.0876,0.32596,-0.08626,-0.002323,0.001002,30,-15,0,5,0,"NULL");
    camera camConfig3 = camera(1084.32,1084.05,339.53,231.71,-0.0876,0.32596,-0.08626,-0.002323,0.001002,0,-45,0,5,0,"NULL");
    camera camConfig4 = camera(1084.32,1084.05,339.53,231.71,-0.0876,0.32596,-0.08626,-0.002323,0.001002,30,-15,0,5,0,"NULL");
    std::vector<cv::Rect> camroi_1 = {cv::Rect(100,100,100,100),cv::Rect(200,200,100,100)};
    std::vector<cv::Rect> camroi_2 = {cv::Rect(100,100,100,100),cv::Rect(200,200,100,100)};
  void init_from_file() {
    std::ifstream ifs;
    ifs.open(configPath);
    if (!ifs.is_open()) {
      LOG(WARNING) << "config file not found! use default values and saving...";
      write_to_file();
      return;
    }
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::Value root;

    if (!Json::parseFromStream(readerBuilder, ifs, &root, &errs)) {
      LOG(WARNING) << "reader parse error: " << errs;
      LOG(WARNING) << "config file load error! use default values.";
      return;
    }
    // config
      Json::Value config = root["config"];
      run_mode = config["run_mode"].asInt();
      show_origin = config["show_origin"].asBool();
      show_armor_box = config["show_armor_box"].asBool();
      show_light_box = config["show_light_bo"].asBool();
      show_class_id = config["show_class_id"].asBool();
      show_net_box = config["show_net_box"].asBool();
      wait_uart = config["wait_uart"].asBool();
      use_pnp = config["use_pnp"].asBool();
      use_video = config["use_video"].asBool();
      save_video = config["save_video"].asBool();
      show_light_blobs = config["show_light_blobs"].asBool();
      save_labelled_boxes = config["save_labelled_boxes"].asBool();
      show_pnp = config["show_pnp"].asBool();
      log_send_target = config["log_send_target"].asBool();
      show_energy_extra = config["show_energy_extra"].asBool();
      show_energy = config["show_energy"].asBool();
      save_mark = config["save_mark"].asBool();
      show_process = config["show_process"].asBool();
      show_info = config["show_info"].asBool();
      show_mcu_info = config["show_mcu_info"].asBool();
      serial_port = config["uart_port"].asString();
      video_path = config["video_path"].asString();
      camera_sn = config["camera_sn"].asString();
      has_show = show_origin || show_armor_box || show_light_box ||
                 show_light_blobs || (show_pnp && use_pnp) || show_energy ||
                 show_energy_extra || show_process || show_net_box;
    // data
      Json::Value data = root["config_data"];
      ARMOR_CAMERA_GAIN = data["ARMOR_CAMERA_GAIN"].asInt();
      ARMOR_CAMERA_EXPOSURE = data["ARMOR_CAMERA_EXPOSURE"].asInt();
      ENERGY_CAMERA_GAIN = data["ENERGY_CAMERA_GAIN"].asInt();
      ENERGY_CAMERA_EXPOSURE = data["ENERGY_CAMERA_EXPOSURE"].asInt();
      ENEMY_COLOR = data["ENEMY_COLOR"].asInt();
    // camera
    Json::Value camera = root["camera"];
    camConfig1.camera_sn = camera["sn_1"].asString();
    camConfig1.fx = camera["fx_1"].asDouble();
    camConfig1.fy = camera["fy_1"].asDouble();
    camConfig1.cx = camera["cx_1"].asDouble();
    camConfig1.cy = camera["cy_1"].asDouble();
    camConfig1.k1 = camera["k1_1"].asDouble();
    camConfig1.k2 = camera["k2_1"].asDouble();
    camConfig1.p1 = camera["p1_1"].asDouble();
    camConfig1.p2 = camera["p2_1"].asDouble();
    camConfig1.k3 = camera["k3_1"].asDouble();
    camConfig1.roi_height = camera["roi_height_1"].asInt64();
    camConfig1.roi_width = camera["roi_width_1"].asInt64();
    camConfig1.roi_offset_x = camera["roi_offset_x_1"].asInt64();
    camConfig1.roi_offset_y = camera["roi_offset_y_1"].asInt64();
    camConfig1.roll = camera["camera1_roll"].asInt();
    camConfig1.yaw = camera["camera1_yaw"].asInt();
    camConfig1.pitch = camera["camera1_pitch"].asInt();
    camConfig1.coordinate_x = camera["camera1_x"].asInt();
    camConfig1.coordinate_y = camera["camera1_y"].asInt();
    camConfig1.init();
    camConfig2.camera_sn = camera["sn_2"].asString();
    camConfig2.fx = camera["fx_2"].asDouble();
    camConfig2.fy = camera["fy_2"].asDouble();
    camConfig2.cx = camera["cx_2"].asDouble();
    camConfig2.cy = camera["cy_2"].asDouble();
    camConfig2.k1 = camera["k1_2"].asDouble();
    camConfig2.k2 = camera["k2_2"].asDouble();
    camConfig2.p1 = camera["p1_2"].asDouble();
    camConfig2.p2 = camera["p2_2"].asDouble();
    camConfig2.k3 = camera["k3_2"].asDouble();
    camConfig2.roi_height = camera["roi_height_2"].asInt64();
    camConfig2.roi_width = camera["roi_width_2"].asInt64();
    camConfig2.roi_offset_x = camera["roi_offset_x_2"].asInt64();
    camConfig2.roi_offset_y = camera["roi_offset_y_2"].asInt64();
    camConfig2.roll = camera["camera2_roll"].asInt();
    camConfig2.yaw = camera["camera2_yaw"].asInt();
    camConfig2.pitch = camera["camera2_pitch"].asInt();
    camConfig2.coordinate_x = camera["camera2_x"].asInt();
    camConfig2.coordinate_y = camera["camera2_y"].asInt();
    camConfig2.init();
    camConfig3.camera_sn = camera["sn_3"].asString();
    camConfig4.camera_sn = camera["sn_4"].asString();
    ifs.close();
  }


  void write_to_file() const {

      Json::Value root;
      // config
      Json::Value config;
      config["run_mode"] = run_mode;
      config["show_origin"] = show_origin;
      config["show_armor_box"] = show_armor_box;
      config["show_light_box"] = show_light_box;
      config["show_net_box"] = show_net_box;
      config["show_class_id"] = show_class_id;
      config["wait_uart"] = wait_uart;
      config["use_pnp"] = use_pnp;
      config["use_video"] = use_video;
      config["save_video"] = save_video;
      config["show_light_blobs"] = show_light_blobs;
      config["save_labelled_boxes"] = save_labelled_boxes;
      config["show_pnp"] = show_pnp;
      config["log_send_target"] = log_send_target;
      config["show_energy_extra"] = show_energy_extra;
      config["show_energy"] = show_energy;
      config["save_mark"] = save_mark;
      config["show_process"] = show_process;
      config["show_info"] = show_info;
      config["show_mcu_info"] = show_mcu_info;
      config["uart_port"] = serial_port;
      config["video_path"] = video_path;
      config["camera_sn"] = camera_sn;
      config["ignore_types"].clear();
      for (auto &item : ignore_types) {
          config["ignore_types"].append(item);
      }

      // data
      Json::Value data;
      data["ARMOR_CAMERA_GAIN"] = ARMOR_CAMERA_GAIN;
      data["ARMOR_CAMERA_EXPOSURE"] = ARMOR_CAMERA_EXPOSURE;
      data["ENERGY_CAMERA_GAIN"] = ENERGY_CAMERA_GAIN;
      data["ENERGY_CAMERA_EXPOSURE"] = ENERGY_CAMERA_EXPOSURE;
      data["ENEMY_COLOR"] = ENEMY_COLOR;

      // camera
    Json::Value camera;
    camera["sn_1"] = camConfig1.camera_sn;
    camera["fx_1"] = camConfig1.fx;
    camera["fy_1"] = camConfig1.fy;
    camera["cx_1"] = camConfig1.cx;
    camera["cy_1"] = camConfig1.cy;
    camera["k1_1"] = camConfig1.k1;
    camera["k2_1"] = camConfig1.k2;
    camera["p1_1"] = camConfig1.p1;
    camera["p2_1"] = camConfig1.p2;
    camera["k3_1"] = camConfig1.k3;
    camera["roi_height_1"] = camConfig1.roi_height;
    camera["roi_width_1"] = camConfig1.roi_width;
    camera["roi_offset_x_1"] = camConfig1.roi_offset_x;
    camera["roi_offset_y_1"] = camConfig1.roi_offset_y;
    camera["camera1_pitch"] = camConfig1.pitch;
    camera["camera1_yaw"] = camConfig1.yaw;
    camera["camera1_roll"] = camConfig1.roll;
    camera["camera1_x"] = camConfig1.coordinate_x;
    camera["camera1_y"] = camConfig1.coordinate_y;
    camera["sn_2"] = camConfig2.camera_sn;
    camera["fx_2"] = camConfig2.fx;
    camera["fy_2"] = camConfig2.fy;
    camera["cx_2"] = camConfig2.cx;
    camera["cy_2"] = camConfig2.cy;
    camera["k1_2"] = camConfig2.k1;
    camera["k2_2"] = camConfig2.k2;
    camera["p1_2"] = camConfig2.p1;
    camera["p2_2"] = camConfig2.p2;
    camera["k3_2"] = camConfig2.k3;
    camera["roi_height_2"] = camConfig2.roi_height;
    camera["roi_width_2"] = camConfig2.roi_width;
    camera["roi_offset_x_2"] = camConfig2.roi_offset_x;
    camera["roi_offset_y_2"] = camConfig2.roi_offset_y;
    camera["camera2_pitch"] = camConfig2.pitch;
    camera["camera2_yaw"] = camConfig2.yaw;
    camera["camera2_roll"] = camConfig2.roll;
    camera["camera2_x"] = camConfig2.coordinate_x;
    camera["camera2_y"] = camConfig2.coordinate_y;
    camera["sn_3"] = camConfig3.camera_sn;
    camera["sn_4"] = camConfig4.camera_sn;
    root["config"] = config;
    root["config_data"] = data;
    root["camera"] = camera;
    std::ofstream ofs;
    ofs.open(configPath);
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(root, &ofs);
    ofs.close();
  }
};

typedef struct {
  int from[2];
  int to[2];
} mapping_point_t;    // 一对点的映射

typedef struct {
  uint64_t count;     // 映射点对数
  mapping_point_t *points;
} mapping_t;          // 所有映射

class mapping {
public:
  mapping_t mapping_data_m = {0, nullptr};
  std::string filename_default = "mapping.bin";
  std::string filename = filename_default;

  mapping() {
    load_from_file();
  }

  explicit mapping(std::string target) : filename(std::move(target)) {
    load_from_file();
  }

  mapping_t *get_mapping() { return &mapping_data_m; }

  std::vector<std::pair<std::vector<int>, std::vector<int>>> get_mapping_vectors() const {
    std::vector<std::pair<std::vector<int>, std::vector<int>>> data;
    for (uint64_t i = 0; i < mapping_data_m.count; i++)
      data.emplace_back(
        std::pair<std::vector<int>, std::vector<int>>(
          {
            std::vector<int>({
                               mapping_data_m.points[i].from[0],
                               mapping_data_m.points[i].from[1]
                             }),
            std::vector<int>({
                               mapping_data_m.points[i].to[0],
                               mapping_data_m.points[i].to[1]
                             })
          })
      );
    return data;
  }

  std::vector<std::vector<int>> get_mapping_vectors_from() const {
    std::vector<std::vector<int>> data;
    for (uint64_t i = 0; i < mapping_data_m.count; i++)
      data.emplace_back(std::vector<int>(
        {
          mapping_data_m.points[i].from[0],
          mapping_data_m.points[i].from[1]
        })
      );
    return data;
  }

  std::vector<std::vector<int>> get_mapping_vectors_to() const {
    std::vector<std::vector<int>> data;
    for (uint64_t i = 0; i < mapping_data_m.count; i++)
      data.emplace_back(std::vector<int>(
        {
          mapping_data_m.points[i].to[0],
          mapping_data_m.points[i].to[1]
        })
      );
    return data;
  }

  void make_free() {
    if (!mapping_data_m.count) return;
    mapping_data_m.count = 0;
    if (!mapping_data_m.points) return;
    free(mapping_data_m.points);
  }

  void make_malloc(uint64_t count = 0) {
    // 如果之前加载过数据就先释放内存
    make_free();
    // 如果参数设置了count就设置map的count在再说
    if (count) mapping_data_m.count = count;
    mapping_data_m.points = (mapping_point_t *) malloc(sizeof(mapping_point_t) * mapping_data_m.count);
    assert(mapping_data_m.points);
  }

// 格式转换
  void load_from_vector(const std::vector<std::pair<std::vector<int>, std::vector<int>>> &vecs) {
    // 只写入有效的，还要排重
    std::map<std::vector<int>, std::vector<int>> mark;
    uint64_t count = 0;
    for (const auto &p : vecs) {
      if (!(p.first.size() == 2 && p.second.size() == 2)) continue;
      if (mark.find(p.first) != mark.end()) {
        mark[p.first] = p.second;
        continue;
      }
      count++;
      mark.insert(p);
    }
    make_malloc(count);
    mapping_point_t *now = mapping_data_m.points;
    for (const auto &p : mark) {
      if (!(p.first.size() == 2 && p.second.size() == 2)) continue;
      now->from[0] = p.first.at(0), now->from[1] = p.first.at(1);
      now->to[0] = p.second.at(0), now->to[1] = p.second.at(1);
      now++;
    }
  }

// 映射文件格式：
// 前第1个uint64_t大小数据表示映射点对数count
// 后count个mapping_point_t大小数据表示count个映射

// 从文件中加载
  bool load_from_file(const std::string &filename_ = "") {
    if (filename_.length() != 0) filename = filename_;
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) return false;
    // assert(fp);
    // 读第一个 uint64_t 到count
    fread(&mapping_data_m.count, sizeof(uint64_t), 1, fp);
    // // 不能读取到0个数据，不然错误
    // assert(mapping.count);
    make_malloc(mapping_data_m.count);
    // 直接读
    fread(mapping_data_m.points, sizeof(mapping_point_t), mapping_data_m.count, fp);
    fclose(fp);
    return true;
  }

// 写入到文件
  bool write_to_file(const std::string &filename_ = "") {
    if (filename_.length() != 0) filename = filename_;
    FILE *fp = fopen(filename.c_str(), "wb");
    assert(fp);
    fwrite(&mapping_data_m.count, sizeof(mapping_data_m.count), 1, fp);
    fwrite(mapping_data_m.points, sizeof(mapping_point_t), mapping_data_m.count, fp);
    fclose(fp);
    return true;
  }

};

}  // namespace config

}  // namespace rm
void saveVideos(cv::Mat& img,std::string prefix);
extern rm::config::general *config;
extern rm::config::mapping *mapping_data;
#endif //RADAR_RMCONFIG_H
