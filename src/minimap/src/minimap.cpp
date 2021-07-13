//
// Created by Chiro on 2021/3/14.
//

#include <minimap.h>


// 储存额外的文字信息
std::map<std::string, std::string> text_extra;
rm::minimap::map *minimap;

#include <opencv2/opencv.hpp>

namespace rm {

namespace minimap {

void vec_set_data(Vec2i &pt, int &x, int &y) {
  pt[0] = x;
  pt[1] = y;
}

Vec2i vec_get_center(const Vec2i &start, const Vec2i &end) {
  return {(start[0] + end[0]) / 2, (start[1] + end[1]) / 2};
}

}

}