/*===========================================================================*/
/*                               使用本代码的兵种 */
/*===========================================================================*/
/*   _______________   _______________   _______________   _______________   */
/*  |     _____     | |     _  _      | |     ____      | |     _____     |  */
/*  ||   |___ /    || ||   | || |    || ||   | ___|    || ||   |___  |   ||  */
/*  ||     |_ \    || ||   | || |_   || ||   |___ \    || ||      / /    ||  */
/*  ||    ___) |   || ||   |__   _|  || ||    ___) |   || ||     / /     ||  */
/*  ||   |____/    || ||      |_|    || ||   |____/    || ||    /_/      ||  */
/*  |_______________| |_______________| |_______________| |_______________|  */
/*                                                                           */
/*===========================================================================*/

#define LOG_LEVEL LOG_NONE
#include <armor_finder/armor_finder.h>
#include <show_images/show_images.h>
#include "rmconfig.h"
#include <opencv2/highgui.hpp>
#include <utility>

std::map<int, std::string> id2name = {  //装甲板id到名称的map
    {0, "OO"},   {-1, "NO"},  {1, "B1"},    {2, "B2"},  {3, "B3"},
    {4, "B4"},   {5, "B7"},   {6, "B11"},   {7, "B10"}, {8, "B10_B"},
    {9, "R1"},   {10, "R2"},  {11, "R3"},   {12, "R4"}, {13, "R7"},
    {14, "R11"}, {15, "R10"}, {16, "R10_B"}};

std::map<std::string, int> name2id = {  //装甲板名称到id的map
    {"OO", 0},   {"NO", -1},  {"B1", 1},    {"B2", 2},  {"B3", 3},
    {"B4", 4},   {"B7", 5},   {"B11", 6},   {"B10", 7}, {"B10_B", 8},
    {"R1", 9},   {"R2", 10},  {"R3", 11},   {"R4", 12}, {"R7", 13},
    {"R11", 14}, {"R10", 15}, {"R10_B", 16}};

std::map<std::string, int> prior_blue = {
    {"B8", 0}, {"B1", 1}, {"B3", 2}, {"B4", 2}, {"B5", 2},
    {"B7", 3}, {"B2", 4}, {"R8", 5}, {"R1", 6}, {"R3", 7},
    {"R4", 7}, {"R5", 7}, {"R7", 8}, {"R2", 9}, {"NO", 10},
};

std::map<std::string, int> prior_red = {
    {"R8", 0}, {"R1", 1}, {"R3", 2}, {"R4", 2}, {"R5", 2},
    {"R7", 3}, {"R2", 4}, {"B8", 5}, {"B1", 6}, {"B3", 7},
    {"B4", 7}, {"B5", 7}, {"B7", 8}, {"B2", 9}, {"NO", 10},
};

ArmorFinder::ArmorFinder(const int &color)
:
      enemy_color(color),
      state(SEARCHING_STATE),  //默认为searching模式
      radar_pnp(config->camConfig1,config->camConfig2)
      {
}

void ArmorFinder::run(cv::Mat &src,cv::Mat &map) {    // 自瞄主函数
    if (stateSearchingTarget(src)) {
      std::vector<cv::Point2d> boxes;
      for (auto & i:this->target_box)
      {
        if(i.rect!= cv::Rect2d(0,0,0,0))
        {
          boxes.push_back(this->radar_pnp.radar_solvePnp(i.rect,radar_pnp.camera_1));
        }
        else
        {
          boxes.emplace_back(0,0);
        }
        boxes = this->wave_fliter(boxes);
      }
      show_pnp("map",map,boxes);
    } else {
      for (auto i = 0; i < target_box.size(); i++) {
        if (target_box[i].rect != cv::Rect2d()) {
          last_box[i] = target_box[i];
        }
      }
    }
    if (config->show_armor_box) {  // 根据条件显示当前目标装甲板
//        showArmorBox("box", src, target_box);
//        cv::waitKey(1);
    }
}

float ArmorFinder::getPointLength(cv::Point_<float> point) {
  return std::sqrt(point.x*point.x+point.y*point.y);
}
std::vector<cv::Point2d> ArmorFinder::wave_fliter(std::vector<cv::Point2d> now_points)
{
  std::vector<cv::Point2d> fliter_points;
  for (int i = 0;i<now_points.size();i++)
  {
    if (now_points.at(i)==cv::Point2d(0,0))
    {
      for (int j = 0; j < points.at(i).filtering.size(); ++j) {
          this->points.at(i).filtering.push(cv::Point2d(0,0));
      }
          fliter_points.push_back(now_points[i]);
    }
    else
    {
      if (this->points.at(i).filtering[0]==cv::Point2d(0,0))
      {
        this->points.at(i).filtering.push(now_points.at(i));
        fliter_points.push_back(now_points.at(i));
      }
      else
      {
        auto ave_p = this->get_average(i);
        if (this->getPointLength(ave_p-now_points.at(i))<20) {
          this->points.at(i).filtering.push(now_points.at(i));
        }
        fliter_points.push_back(ave_p);
      }
    }
  }
  return fliter_points;
}

