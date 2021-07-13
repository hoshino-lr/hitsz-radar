/*
 * @filename    : minimap.hpp
 * @desp        : 小地图UI实现
 * @usage       : ...
 * @author      : Chiro
 * @date        : 2021/03/14
 */


#ifndef RADAR_MINIMAP_H
#define RADAR_MINIMAP_H

#include <chrono>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <utility>
#include <vector>
#include <cstdio>
// 引入一些数学常数的操作
#define _MATH_DEFINES_

#include <cmath>

#undef _MATH_DEFINES_

#include <opencv2/opencv.hpp>
#include <rmrefs.h>

#include "./refs.h"

#ifndef USUPER
// 继承基类构造函数的语法糖
#define USUPER(CLASS) \
 public:              \
  using CLASS::CLASS;
// 异常处理语法糖
#endif
#ifndef throwif
#define throwif(x, exception) \
  do {                        \
    if (x) throw exception;   \
  } while (0)
#endif
#ifndef throwifminus
#define throwifminus(x, exception) throwif(x < 0, exception)
#endif

// 储存额外的文字信息
extern std::map<std::string, std::string> text_extra;

namespace rm {

// 物体代码枚举
enum class object_type {
  none = 0,        // 保留
  hero = 1,        // 英雄
  engineer = 2,    // 工程
  infantry_3 = 3,  // 步兵
  infantry_4 = 4,  // 步兵
  infantry_5 = 5,  // 步兵
  aerial = 6,      // 空中
  radar = 9,       // 雷达
  /* 上面是可以生成ID的代码，下面的不行 */
  sentinel = 7,  // 哨兵机器人
  /* 上面的都是机器人，下面的不是 */
  outposts = 8,        // 前哨站
  energy_organs = 10,  // 能量机关
  base = 11,            // 基地
};
// 队伍代码枚举
enum class team_type {
  red = 0x100,
  blue = 0x110,
  /* 下面两个不用于生成代码 */
  us,
  enemy,
};

// 发送、接受者ID等于队伍代码枚举加机器人代码枚举
inline uint16_t robot_get_id(team_type team, object_type robot_type) {
  return (uint16_t) team + (uint16_t) robot_type;
}

namespace minimap {

// 默认配置
const uint8_t line_width = 4;
// 以radar.png图片为准
const uint16_t map_width = 823;
const uint16_t map_height = 714;
// const uint16_t map_width = 640;
// const uint16_t map_height = 480;
const uint16_t animation_round_width = 20;

class cv_colors_t {
public:
  cv::Scalar red = cv::Scalar(0, 0, 255);
  cv::Scalar yellow = cv::Scalar(0, 255, 255);
//    cv::Scalar blue = cv::Scalar(255, 0, 0);
  cv::Scalar green = cv::Scalar(0, 255, 0);
  cv::Scalar black = cv::Scalar(0, 0, 0);
  cv::Scalar orange = cv::Scalar(0, 165, 255);
  cv::Scalar purple = cv::Scalar(128, 0, 128);
  cv::Scalar pink = cv::Scalar(203, 192, 255);
  cv::Scalar cyan = cv::Scalar(255, 255, 0);
  cv::Scalar white = cv::Scalar(255, 255, 255);
};

/*
 * @desp: 获取数字转字符串的值，补零
 */
template<typename T>
std::string to_string_n(T a, size_t n) {
  static char buf[512];
  static char buf2[512];
  strcpy(buf2, (std::string("%0") + std::to_string(n) + std::string("d")).c_str());
  sprintf(buf, buf2, a);
  return std::string(buf);
}

using cv::Vec2i;

void vec_set_data(Vec2i &pt, int &x, int &y);

Vec2i vec_get_center(const Vec2i &start, const Vec2i &end);

// 所有异常
namespace exceptions {
/*
 * @brief 异常基类
 * 能把继承了这个类的类的what()直接打印出类名称。
 */
const size_t BUFSIZ_EXCEPTION = 1024;

class base : public std::exception {
public:
  std::string message;

  base() = default;

  explicit base(std::string msg) : message(std::move(msg)) {}

  base *see_message(std::string message_) {
    this->message = std::move(message_);
    return this;
  }

  const char *what() const noexcept override {
    static std::stringstream ss;
    static char buf[BUFSIZ_EXCEPTION];
    ss.str("");
    ss << "[" << typeid(*this).name() << "] : " << this->message;
    strcpy(buf, ss.str().c_str());
    return (const char *) buf;
  }
};

#ifndef exception_create
#define exception_create(name) \
  class name : public base {   \
    USUPER(base)               \
  }
#endif

exception_create(value_error);

exception_create(arg_error);

} // namespace exceptions

// 颜色类型枚举
enum class color_type {
  main = 0,  // 主色，自己队伍颜色
  yellow,
  green,
  orange,
  purple,
  pink,
  cyan,  // 青色
  black,
  white,
  /* 下面在UI文档未定义 */
  empty  // 透明，到时候不绘制
};

static uint16_t shape_name_code = 0;  // 自动命名，自增

/*
 * @desp: 图形基类
 * 包含所有图形，不做继承等操作。
 */
class shape {
public:
  // 图形类型枚举，强类型枚举
  enum class shape_type {
    empty = 0,  // 空
    line,       // 1, 直线
    rect,       // 2, 矩形
    circle,     // 3, 正圆
    elliptic,   // 4, 椭圆
    arc,        // 5, 圆弧
    text        // 6, 字符
  };
  // 操作枚举
  enum class operate {
    empty = 0, add, modify, delete_
  };

  shape::shape_type graphic_type = shape::shape_type::empty;
  Vec2i start;
  Vec2i end;
  operate operate_type = operate::empty;
  color_type color = color_type::empty;
  std::string name;      // 图形名字，类似"001"
  int radius = 0;          // 字体大小或者半径，正圆、字符用到
  Vec2i angle;         // 弧度，圆弧用到
  std::string text_;     // 文字，字符用到
  int width = line_width;  // 线宽，图像用到
  uint8_t layer = 1;     // 图层

  shape() { this->name = to_string_n<uint16_t>(shape_name_code++, 2); }

  static shape empty() {
    shape s = shape();
    s.graphic_type = shape::shape_type::empty;
    return s;
  }

  static shape line(const Vec2i &start_, const Vec2i &end_,
                    color_type color_ = color_type::main, uint8_t layer_ = 1,
                    uint8_t width_ = line_width) {
    shape s = shape();
    s.graphic_type = shape::shape_type::line;
    s.start = start_, s.end = end_, s.width = width_, s.color = color_;
    return s;
  }

  static shape rect(const Vec2i &start_, const Vec2i &end_,
                    color_type color_ = color_type::main, uint8_t layer_ = 1,
                    uint8_t width_ = line_width) {
    shape s = shape();
    s.graphic_type = shape::shape_type::rect;
    s.start = start_, s.end = end_, s.width = width_, s.color = color_;
    return s;
  }

  static shape circle(const Vec2i &start_, int radius_,
                      color_type color_ = color_type::main,
                      uint8_t layer_ = 1, uint8_t width_ = line_width) {
    shape s = shape();
    s.graphic_type = shape::shape_type::circle;
    s.start = start_, s.radius = radius_, s.width = width_, s.color = color_;
    return s;
  }

  static shape elliptic(const Vec2i &start_, const Vec2i &end_,
                        color_type color_ = color_type::main,
                        uint8_t layer_ = 1, uint8_t width_ = line_width) {
    shape s = shape();
    s.graphic_type = shape::shape_type::elliptic;
    s.start = start_, s.end = end_, s.width = width_, s.color = color_;
    return s;
  }

  static shape arc(const Vec2i &start_, const Vec2i &end_, const Vec2i &angle_,
                   color_type color_ = color_type::main, uint8_t layer_ = 1,
                   uint8_t width_ = line_width) {
    shape s = shape();
    s.graphic_type = shape::shape_type::arc;
    s.start = start_, s.end = end_, s.angle = angle_, s.width = width_,
    s.color = color_;
    return s;
  }

  static shape text(const Vec2i &start_, const std::string &text_, int radius_,
                    color_type color_ = color_type::main, uint8_t layer_ = 1,
                    uint8_t width_ = 0) {
    shape s = shape();
    if (width_ == 0) width_ = radius_ / 10;
    s.graphic_type = shape::shape_type::text;
    s.start = start_, s.text_ = text_, s.radius, s.width = width_,
                                                 s.color = color_;
    return s;
  }

  graphic_data_struct_t pack() const {
    graphic_data_struct_t data;
    // 名字最长三个字节...
    strcpy((char *) data.graphic_name, this->name.substr(0, 2).c_str());
    // printf("this->name = %s, data.graphic_name = %s\n", this->name.c_str(), (char *) data.graphic_name);
    data.operate_type = (uint8_t) this->operate_type;
    data.graphic_type = (uint8_t) this->graphic_type;
    data.layer = (uint8_t) this->layer;
    data.color = (uint8_t) this->color;
    data.start_angle = (uint32_t) this->angle[0];
    data.end_angle = (uint32_t) this->angle[1];
    data.width = (uint32_t) this->width;
    data.start_x = (uint32_t) this->start[0];
    data.start_y = (uint32_t) this->start[1];
    data.radius = (uint32_t) this->radius;
    data.end_x = (uint32_t) this->end[0];
    data.end_y = (uint32_t) this->end[1];

    if (this->graphic_type == shape::shape_type::text)
      // 把文字信息储存到额外的全局变量里头
      // text_extra[std::string((char *) data.graphic_name)] = this->text_;
      text_extra[this->name] = this->text_;
    return data;
  }

  friend std::ostream &operator<<(std::ostream &os, shape &s) {
    static std::map<shape_type, const char *> shape_type_name = {
            {shape_type::empty,    "空"},
            {shape_type::line,     "直线"},
            {shape_type::rect,     "矩形"},
            {shape_type::circle,   "正圆"},
            {shape_type::elliptic, "椭圆"},
            {shape_type::text,     "文字"},
    };

    os << "[" << shape_type_name[s.graphic_type] << "_" << s.name << "]@"
       << s.start;
    return os;
  }
};

class rect {
public:
  Vec2i start, end;

  rect() : start(Vec2i(0, 0)), end(Vec2i(0, 0)) {}

  rect(int x1_, int y1_, int x2_, int y2_) {
    vec_set_data(this->start, x1_, y1_);
    vec_set_data(this->end, x2_, y2_);
  }

  template<class T>
  static rect from_vector(const std::vector<T> &vecs) {
    if (vecs.size() < 4) throw exceptions::arg_error();
    return {Vec2i(vecs.at(0), vecs.at(1)), Vec2i(vecs.at(2), vecs.at(3))};
  }

  rect(const Vec2i &s, const Vec2i &e) : start(s), end(e) {}

  rect clone() const {
    return {this->start, this->end};
  }

  inline Vec2i get_center() const { return vec_get_center(this->start, this->end); }

  inline int &get_width() const { return (this->end - this->start)[0]; }

  inline int &get_height() const { return (this->end - this->start)[1]; }

  inline Vec2i get_start() const { return this->start; }

  inline Vec2i get_end() const { return this->end; }

  inline int max() const {
    return this->get_height() > this->get_width() ? this->get_height()
                                                  : this->get_width();
  }

  inline int min() const {
    return this->get_height() < this->get_width() ? this->get_height()
                                                  : this->get_width();
  }

  rect get_margin(int margin) const {
    Vec2i p{margin, margin};
    return {this->start + p, this->end + p};
  }

  rect get_padding(int padding) const {
    Vec2i p{padding, padding};
    return {this->start - p, this->end - p};
  }

//  void move(const Vec2i &pos) {
//    move(pos);
//  }

//  void move(const Vec2i &&pos) {
  void move(const Vec2i pos) {
    int width = this->get_width();
    int height = this->get_height();
//    std::cout << "width: " << width << ", height: " << height << std::endl;
//    std::cout << "pos: " << pos << std::endl;
//    std::cout << "#0\tstart: " << this->start << ", end: " << this->end << std::endl;
    this->start = Vec2i(pos[0] - width / 2, pos[1] - height / 2);
    this->end = this->start + Vec2i(width, height);
//    std::cout << "#1\tstart: " << this->start << ", end: " << this->end << std::endl;
  }

  rect move_to(const Vec2i &pos) const {
    Vec2i d = pos - this->get_center();
    return {this->start + d, this->end + d};
  }

  bool operator==(const rect &a) const {
    return (this->start == a.start) && (this->end == a.end);
  }

  friend std::ostream &operator<<(std::ostream &os, rect &r) {
    os << "(" << r.start << ", " << r.end << ")";
    return os;
  }
};

class animation {
public:
  // 动画类型枚举
  enum class animation_type {
    empty = 0,           // 无操作
    circle_expand,       // 扩展圆
    circle_expand_fast,  // 快速扩展圆
    spikes,              // 尖刺
    blade_arc            // 刃状弧
  };
  // 动画过程类型枚举
  enum class time_type_ {
    once,    // 单次
    cycle,   // 循环
    forever  // 一直循环
  };
  // 动画共有属性
  animation_type type = animation_type::empty;
  time_type_ time_type = time_type_::once;

  // TODO: 形成动画链
  animation *previous = nullptr;
  animation *next = nullptr;
  // 计时
  std::chrono::system_clock::time_point start_time;
  // 时长默认1000ms
  std::chrono::milliseconds duration =
          std::chrono::milliseconds(1000);  // 周期或者时长
  // 是否可用，即在下一个循环会不会被删除
  bool is_avaliable = true;
  // 已经开始计时的动画才能获取到shape
  bool is_started = false;

  // 动画参数
  // 方向：攻击方向和被攻击方向，0 ~ 360度
  uint16_t direction = 0;
  rect box;
  uint16_t width = line_width;
  color_type color;

  // 开始计时
  animation *start() {
    this->start_time = std::chrono::system_clock::now();
    this->is_started = true;
    return this;
  }

  // 重新开始计时
  animation *restart() { return start(); }

  // 取消这个动画
  animation *cancel() {
    this->is_avaliable = false;
    return this;
  }

  // 查看是否可用
  inline bool avaliable() const { return this->is_avaliable; }

  // 更新可用状态
  void update() {
    if (!this->avaliable()) return;
    if (this->time_type == animation::time_type_::once) {
      if (this->get_time() > this->duration) this->is_avaliable = false;
    }
  }

  // 在尾部添加动画
  animation *push_back(animation *ani) {
    if (ani) this->next = ani;
    return this;
  }

  // 在头部添加动画
  animation *push_front(animation *ani) {
    if (ani) this->previous = ani;
    return this;
  }

  animation *get_head() {
    animation *p = this;
    while (p->previous) p = p->previous;
    return p;
  }

  animation *get_tail() {
    animation *p = this;
    while (p->next) p = p->next;
    return p;
  }

  animation(animation_type t, rect box_) : type(t), box(std::move(box_)), color(color_type::main) {
    this->init_type();
  }

  // 带方向初始化
  animation(animation_type t, rect box_, uint16_t direction_)
          : type(t), box(box_), direction(direction_), color(color_type::main) {
    this->init_type();
  }

  ~animation() {
    // 从头删除到自己，然后从尾部删除到自己，最后删除自己
    if (this->previous == nullptr and this->next == nullptr) return;
    animation *p = this->get_head();
    while (p != this) {
      p->previous = p->next = nullptr;
      delete p;
    };
    p = this->get_tail();
    while (p != this) {
      p->previous = p->next = nullptr;
      delete p;
    }
  }

  // 获取当前动画的所有形状
  std::vector<shape> get() {
    std::vector<shape> data;
    Vec2i center = this->box.get_center();
    uint16_t box_max = this->box.max();
    if (!this->is_avaliable || !this->is_started) return data;
    if (this->type == animation_type::empty) {
      // 空动画
    } else if (this->type == animation_type::circle_expand) {
      // 圆形展开
      auto r = (uint16_t) (box_max * (this->get_percent() * 0.5 + 0.5));
      // 放两个环
      data.emplace_back(
              shape::circle(center, r, color, (uint8_t) this->type, width));
      data.emplace_back(
              shape::circle(center, r / 2, color, (uint8_t) this->type, width));
    } else if (this->type == animation_type::circle_expand_fast) {
      // 快速圆形展开
      // 就放一个环
      auto r = (uint16_t) (box_max * this->get_percent());
      data.emplace_back(shape::circle(center, r, color, width));
    } else if (this->type == animation_type::spikes) {
      // 尖刺
      // 画三角形，就是画三条直线
      Vec2i p{(uint16_t) (sin((double) this->direction / 360 * 2 * M_PI) * box_max),
              (uint16_t) (cos((double) this->direction / 360 * 2 * M_PI) * box_max)},
              p1{
              (uint16_t) (sin((double) (this->direction + 45) / 360 * 2 * M_PI) *
                          box_max * 0.8),
              (uint16_t) (cos((double) (this->direction + 45) / 360 * 2 * M_PI) *
                          box_max * 0.8)},
              p2{(uint16_t) (sin((double) (this->direction - 45) / 360 * 2 * M_PI) *
                             box_max * 0.8),
                 (uint16_t) (cos((double) (this->direction - 45) / 360 * 2 * M_PI) *
                             box_max * 0.8)};
      p += center;
      p1 += center;
      p2 += center;
      data.emplace_back(shape::line(p, p1, color, (uint8_t) this->type, width));
      data.emplace_back(
              shape::line(p1, p2, color, (uint8_t) this->type, width));
      data.emplace_back(shape::line(p2, p, color, (uint8_t) this->type, width));
    } else if (this->type == animation_type::blade_arc) {
      // 刃状弧
      Vec2i p(
              (uint16_t) (sin((double) this->direction / 360 * 2 * M_PI) * box_max),
              (uint16_t) (cos((double) this->direction / 360 * 2 * M_PI) * box_max));
      p += center;
      auto r = (uint16_t) ((double) box_max * 0.3);
      uint16_t d = (this->direction + 180) % 360;
      uint16_t limit = 50;
      data.emplace_back(shape::arc(p, Vec2i(r, r), Vec2i(d - limit, d + limit),
                                   color, (uint8_t) this->type, width));
    }
    return data;
  }

private:
  void init_type() {
    // 设置各种动画的初始参数
    if (this->type == animation_type::empty) {
      // 空动画不可用
      this->is_avaliable = false;
      this->time_type = time_type_::forever;
    } else if (this->type == animation_type::circle_expand) {
      // 圆形展开是循环的
      this->time_type = time_type_::cycle;
    } else if (this->type == animation_type::circle_expand_fast) {
      // 快速圆形展开只有一次
      this->time_type = time_type_::once;
      this->duration = std::chrono::milliseconds(350);
    } else if (this->type == animation_type::spikes) {
      // 尖刺一直存在直到状态取消
      this->time_type = time_type_::forever;
    } else if (this->type == animation_type::blade_arc) {
      // 刃状弧只持续一次
      this->time_type = time_type_::once;
      this->duration = std::chrono::milliseconds(200);
    }
  }

  inline std::chrono::milliseconds get_time() const {
    auto now = std::chrono::system_clock::now();
    auto past = now - this->start_time;
    if (this->time_type == time_type_::cycle) past %= this->duration;
    return std::chrono::duration_cast<std::chrono::milliseconds>(past);
  }

  inline double get_percent() {
    return (double) this->get_time().count() / this->duration.count();
  }
};

/*
 * @desp: 容器基类
 * 容器内的形状的位置是相对这个容器的位置。
 */
class frame {
private:
  void vector_shape_push(std::vector<shape> &data, shape s) const {
    s.start += box.get_start();
    // 圆弧和椭圆的end不做偏移
    if (s.graphic_type != shape::shape_type::arc and
        s.graphic_type != shape::shape_type::elliptic)
      s.end += box.get_start();
    data.emplace_back(s);
  }

public:
  // 默认对象大小
  const rect object_box_default = rect(0, 0, 35, 35);
  rect box = object_box_default;
  // std::vector<animation> animations;
  std::map<animation::animation_type, animation> animations;
  // frame自带shape不会动但是可以改变
  // 图形名字变成"C01"/"R01"，后两位表示机器代码
  std::vector<shape> self;

  // 子容器
  // std::vector<>
  explicit frame(rect box_) : box(std::move(box_)) {}

  frame() = default;

  void set_box(const rect &box_) { this->box = box_; }

  // 设置动画前请先设置好box
  void add_animation(animation ani) {
    // 如果没设置rect就设置到这个frame大小
    if (ani.box == rect(0, 0, 0, 0)) ani.box = this->box;
    // 然后打开
    ani.start();
    this->animations.insert(
            std::pair<animation::animation_type, animation>(ani.type, ani));
  }

  // 给这个frame添加静态的图像
  void add_shape(shape s) {
    // 会被放到图层顶层0
    s.layer = 0;
    // 到取出的时候才会做偏移
    this->self.emplace_back(s);
  }

  void reshape(const rect &box_) { this->box = box_; }

  void move_to(const Vec2i &start) {
    Vec2i e = start - this->box.get_start();
    this->box.start = start;
    this->box.end += e;
  }

  // 获取坐标转换过后的所有形状，顺便检查是否应该删除
  std::vector<shape> get_shapes() {
    std::vector<shape> data;
    for (shape &s : this->self) this->vector_shape_push(data, s);
    std::vector<animation::animation_type> to_remove;
    for (auto &p : this->animations) {
      std::vector<shape> shapes = p.second.get();
      for (shape &s : shapes) this->vector_shape_push(data, s);
      if (!p.second.avaliable()) to_remove.emplace_back(p.first);
    }
    for (auto t : to_remove) this->animations.erase(t);
    return data;
  }
};

// 动作枚举
// action & 0xF0 的部分存在的时候后面必须唯一
enum class actions {
  none = 0x0000,
  alive = 0x0100 << 0 | 0x0001 << 0,
  dead = 0x0100 << 0 | 0x0001 << 1,
  attacking = 0x0100 << 1 | 0x0001 << 2,
  under_attacking = 0x0100 << 2 | 0x0001 << 3,
  buffed = 0x0100 << 3 | 0x0001 << 4,
  moving = 0x0100 << 4 | 0x0001 << 5,
};
// 可信度类型
enum class credibility_type {
  confirmed = 0,      // 确定的
  fuzzy = 1,          // 不确定的
  disappeared = 2,    // 消失了
};

/*
 * @desp: 对象类，继承于frame
 * 代表场地、机器人、飞镖、基地、前哨站等所有场上物体。
 */
class object : public frame {
private:
  uint8_t animations_count() const {
    // 忽视静态的活着、死亡状态
    uint8_t dat = (this->status & 0xFF00) >> 1;
    uint8_t count = 0;
    while (dat) {
      dat >>= 1;
      count++;
    }
    return count;
  };

public:
  std::string name;
  // 当前状态
  uint32_t status = 0;
  // 机器人代码，见UI手册
  // uint16_t code = 0;
  object_type type = object_type::none;
  // 队伍归属
  team_type team = team_type::red;
  // 主要颜色
  color_type color = color_type::main;
  // 可信度
  credibility_type credibility = credibility_type::confirmed;

  bool is_robot() const {
    return this->type > object_type::none &&
           this->type <= object_type::sentinel;
  }

  // object() : frame(object_box_default) {}
  object() = default;

  object(object_type type_, team_type team_, std::string name_) : type(type_), team(team_), name(std::move(name_)) {
    this->box = object_box_default;
    this->init();
  }

  object(object_type type_, team_type team_, std::string name_, const rect &box_)
          : type(type_), team(team_), name(std::move(name_)) {
    this->box = box_;
    this->init();
  }

  object(object_type type_, team_type team_, std::string name_, const Vec2i &size)
          : type(type_), team(team_), name(std::move(name_)) {
    this->box = rect(Vec2i(), size);
    this->init();
  }

  void init() {
//    if (this->team == team_type::us || this->team ==)
    this->update_robot_shape();
    if (this->is_robot()) {
      this->status = (uint32_t) actions::alive;
      // 然后给机器人添加自己显示
    }
    this->update(actions::alive);
  }

  void update_robot_shape() {
    if (!this->is_robot()) return;
    this->self.clear();
    uint16_t min_ = this->box.min();
    if (this->credibility == credibility_type::confirmed) {
      this->add_shape(
              shape::circle(Vec2i(), (uint16_t) ((double) min_ / 2 * 0.5),
                            this->color, 0, 12));
      this->add_shape(shape::text(Vec2i(-4, 6), std::to_string((int) type),
                                  (uint16_t) ((double) this->box.min() / 2 * 0.2),
                                  color_type::white, 0, 2));
    } else if (this->credibility == credibility_type::fuzzy) {
      this->add_shape(
              shape::circle(Vec2i(), (uint16_t) ((double) min_ / 2 * 0.5),
                            this->color, 0, 12));
      this->add_shape(shape::text(Vec2i(-8, 6), std::to_string((int) type),
                                  (uint16_t) ((double) this->box.min() / 2 * 0.2),
                                  color_type::white, 0, 1));
      // 添加个问号
      this->add_shape(shape::text(Vec2i(0, 6), "?",
                                  (uint16_t) ((double) this->box.min() / 2 * 0.2),
                                  color_type::white, 0, 1));
    } else if (this->credibility == credibility_type::disappeared) {
      this->add_shape(
              shape::circle(Vec2i(), (uint16_t) ((double) min_ / 2 * 0.7),
                            this->color, 0, 1));
      this->add_shape(shape::text(Vec2i(-8, 6), std::to_string((int) type),
                                  (uint16_t) ((double) this->box.min() / 2 * 0.2),
                                  color_type::black, 0, 1));
      // 添加个问号
      this->add_shape(shape::text(Vec2i(0, 6), "?",
                                  (uint16_t) ((double) this->box.min() / 2 * 0.2),
                                  color_type::black, 0, 1));
    }
  }

  void update(actions action) {
    if (action == actions::attacking || action == actions::under_attacking ||
        action == actions::moving)
      return;
    auto stat = (uint32_t) action;
    // uint16_t = (uint16_t)((double)this->box.min() / 2 * 0.2);
    // 前位没有的话就直接或，添加动画
    if (!((this->status & 0xFF00) & (stat & 0xFF00))) {
      this->status |= stat;
      // 按照动画个数分配box大小
      if (action == actions::buffed)
        this->add_animation(
                animation(animation::animation_type::circle_expand,
                          this->box.get_margin(animation_round_width *
                                               (1 + this->animations_count()))));
    } else {
      // 有的话就替换状态，更新动画
      // TODO: 更新动画
      this->status = this->status & 0xFF00 + stat & 0x00FF;
      if (action == actions::buffed) {
        auto p =
                this->animations.find(animation::animation_type::circle_expand);
        if (p == this->animations.end());
      }
    }
    // 如果需要给这个物体绘制自身图案（有生死属性）
    // 注意运算顺序啊喂
    if ((this->status & (stat & 0xFF00)) == (stat & 0xFF00))
      this->update_robot_shape();
  }

  void update(actions action, uint16_t direction) {
    if (!(action == actions::attacking || action == actions::under_attacking ||
          action == actions::moving))
      return;
    this->status = (uint32_t) action;
  }

  void dismiss(actions action) { this->status &= (uint32_t) action; }
};

/*
 * @desp: 地图主逻辑
 * 从这里开始。
 */
class map {
public:
  // 更新类型枚举
  enum class update_operate {
    position,    // 位置，参数为vector<int>
    buffed,      // 是否被加强，参数为默认true
    credibility, // 可信度，参数三种
  };

  // 队伍颜色键值对，离谱的是没有红色和蓝色
  std::map<team_type, color_type> team_color = {
          {team_type::red,  color_type::pink},
          {team_type::blue, color_type::cyan}};
  std::map<object_type, std::string> object_type_string = {
          {object_type::none,          "保留"},
          {object_type::hero,          "英雄"},
          {object_type::engineer,      "工程"},
          {object_type::infantry_3,    "步兵1"},
          {object_type::infantry_4,    "步兵2"},
          {object_type::infantry_5,    "步兵3"},
          {object_type::aerial,        "空中"},
          {object_type::sentinel,      "哨兵"},
          {object_type::outposts,      "前哨站"},
          {object_type::base,          "基地"},
          {object_type::energy_organs, "能量机关"},
  };


  std::map<uint16_t, object> robots;
  Vec2i size = Vec2i(map_width, map_height);

  // 添加机器人
  void robot_add(uint16_t id, object robot) {
//    object robot = object(robot_);
    if (robot.team == team_type::us || robot.team == this->team_us)
      robot.color = color_type::main;
    else if (this->team_us == team_type::red) robot.color = color_type::cyan;
    else robot.color = color_type::orange;
    this->robots.insert({id, robot});
//    this->robots[id] = robot;
  }

  // 自己是哪一方
  team_type team_us = team_type::red;

  uint16_t map_robot_get_id(team_type team_, object_type robot_type) const {
    if (team_ == team_type::red or team_ == team_type::blue) return robot_get_id(team_, robot_type);
    if (team_ == team_type::us) return robot_get_id(this->team_us, robot_type);
    return robot_get_id(this->team_us == team_type::red ? team_type::blue : team_type::red, robot_type);
  }

  inline color_type get_team_color(team_type team) {
    return team == this->team_us ? color_type::main : team_color[team];
  }

  // 更新信息：位置
  void robot_update(update_operate operate_type, uint16_t id,
                    const Vec2i &vec) {
    if (this->robots.find(id) == this->robots.end()) return;
    if (operate_type != update_operate::position) return;
    this->robots[id].box.move(vec);
    this->robots[id].update(actions::alive);
  }

  // 更新信息：可信度
  void robot_update(update_operate operate_type, uint16_t id,
                    credibility_type credibility) {
    if (this->robots.find(id) == this->robots.end()) return;
    if (operate_type != update_operate::credibility) return;
    this->robots[id].credibility = credibility;
  }

  // 更新信息：buff
  void robot_update(update_operate operate_type, uint16_t id,
                    bool is_buffed = true) {
    if (this->robots.find(id) == this->robots.end()) return;
    if (operate_type != update_operate::buffed) return;
    if (is_buffed)
      this->robots[id].update(actions::buffed);
    else
      this->robots[id].dismiss(actions::buffed);
  }

  // 获取当前所有形状
  std::vector<graphic_data_struct_t> get() {
    std::vector<shape> shapes = this->get_shapes();
    std::vector<graphic_data_struct_t> data;
    for (const auto &i : shapes) data.emplace_back(i.pack());
    return data;
  }

  std::vector<shape> get_shapes() {
    // 在获取shapes之前需要做一些工作
    shape_name_code = 0;
    text_extra.clear();
    std::vector<shape> data;
    for (auto i : this->robots)
      for (const auto &j : i.second.get_shapes()) data.emplace_back(j);
    return data;
  }

  // 调试用：绘图
  void draw(int wait_time = 0) {
    // static cv::Mat background_g = cv::imread("map.png");
    static cv::Mat background_g = cv::imread("radar.png");
    cv::Mat bg = background_g.clone();
//    cv::resize(background_g, bg, cv::Size(this->size.x, this->size.y));
    cv::Mat src = bg.clone();
    std::vector<graphic_data_struct_t> graphic_data = this->get();
    cv::Mat dst = this->draw_t(bg, graphic_data);
    cv::imshow("mini-map", dst);
    cv::waitKey(wait_time);
  }

  cv_colors_t cv_colors;

  cv::Scalar choose_color(uint32_t color) {
    static std::vector<cv::Scalar> colors = {
            cv_colors.red, cv_colors.yellow, cv_colors.green,
            cv_colors.orange, cv_colors.purple, cv_colors.pink,
            cv_colors.cyan, cv_colors.black, cv_colors.white};
    if (color == 0) {
      if (this->team_us == team_type::red)
        return colors.at(0);
      if (this->team_us == team_type::blue)
        return colors.at(6);  // cyan
    }
    if (1 <= color and color <= 7) return colors.at(color);

    return colors.at(8);
  }

  static std::string text_extra_get(const std::string &name) {
    if (text_extra.find(name) == text_extra.end()) return "";
    return text_extra[name];
  }

  // 绘制单个图形
  void draw_shape_t(cv::Mat &src, graphic_data_struct_t t) {
    std::string graghic_name = reinterpret_cast<const char *>(t.graphic_name);
    uint32_t operate_type = t.operate_type;    //操作形式
    uint32_t graphic_type = t.graphic_type;    //图像形式        //图层
    cv::Scalar color = this->choose_color(t.color);  //颜色
    uint32_t start_angle = t.start_angle;      //圆弧开始角度
    uint32_t end_angle = t.end_angle;          //圆弧结束角度
    uint32_t width = t.width;                  //宽度
    uint32_t start_x = t.start_x;              //起始坐标
    uint32_t start_y = t.start_y;              //起始坐标
    uint32_t radius = t.radius;                //半径
    uint32_t end_x = t.end_x;                  //结束坐标x
    uint32_t end_y = t.end_y;                  //结束坐标y

    int center_x = (int) (start_x + end_x) / 2;
    int center_y = (int) (start_y + end_y) / 2;
    int axes_x = abs((int) (end_x - start_x)) / 2;
    int axes_y = abs((int) (end_y - start_y)) / 2;

    switch (graphic_type) {
      case 0:  // 空图形，不绘制
        break;
      case 1:  // 直线
        cv::line(src, cv::Point(start_x, start_y), cv::Point(end_x, end_y),
                 color, width);
        break;
      case 2:  // 矩形
        cv::rectangle(src, cv::Rect(start_x, start_y, end_x, end_y), color, 1,
                      width, 0);
        break;
      case 3:  // 正圆
        cv::circle(src, cv::Point(start_x, start_y), radius, color, width);
        break;
      case 4:  // 椭圆
        cv::ellipse(src, cv::Point(center_x, center_y),
                    cv::Size(axes_x, axes_y), 0, start_angle, end_angle, color,
                    width);
        break;
      case 5:  // 圆弧
        cv::ellipse(src, cv::Point(start_x, start_y), cv::Size(radius, radius),
                    0, start_angle, end_angle, color, width);
        break;
      default:
        cv::putText(src, text_extra_get(graghic_name), cv::Point(start_x, start_y),
                    cv::FONT_HERSHEY_DUPLEX, 0.4, color, width);
        break;
    }
  }

  // 绘图：结构体参数
  cv::Mat draw_t(cv::Mat src, const std::vector<graphic_data_struct_t> &graphic_data) {
    // 分层
    std::map<uint32_t, std::vector<graphic_data_struct_t>> layers;
    std::priority_queue<uint32_t, std::vector<uint32_t>, std::less<uint32_t> >
            layers_count;
    for (graphic_data_struct_t d : graphic_data) {
      if (layers.find(d.layer) == layers.end()) {
        layers.insert(std::pair<uint32_t, std::vector<graphic_data_struct_t>>(
                (uint32_t) d.layer, std::vector<graphic_data_struct_t>()));
        layers_count.push((uint32_t) d.layer);
      }
      layers[d.layer].emplace_back(d);
    }
    // 绘图，从低到高绘制，图层高的在图层低的上面
    // TODO: 修改图层逻辑
    while (!layers_count.empty()) {
      uint32_t l = layers_count.top();
      layers_count.pop();
      for (auto t : layers[l]) this->draw_shape_t(src, t);
    }
    return src;
#if 0
    //先对图形进行分层
    vector<vector<graphic_data_struct_t>> graphic_layer_[5];
    radar_picture_show = background_picture.clone();
    imshow("radar_station", radar_picture_show);
    for (int i = 0; i < (*graphic_data).size(); i++) {
      switch ((*graphic_data)[i].layer) {
        case 0:
          graphic_layer_->at(0).push_back(graphic_data->at(i));
          break;
        case 1:
          graphic_layer_->at(1).push_back(graphic_data->at(i));
          break;
        case 2:
          graphic_layer_->at(2).push_back(graphic_data->at(i));
          break;
        case 3:
          graphic_layer_->at(3).push_back(graphic_data->at(i));
          break;
        default:
          graphic_layer_->at(4).push_back(graphic_data->at(i));
          break;
      }
    }

    //画图形
    for (int i = graphic_layer_->size() - 1; i >= 0; i--) {
      for (int j = 0; j < graphic_layer_[i].size(); j++) {
        string graghic_name = reinterpret_cast<const char*>(
            graphic_layer_->at(i).at(j).graphic_name);
        uint32_t operate_type =
            graphic_layer_->at(i).at(j).operate_type;  //操作形式
        uint32_t graphic_type =
            graphic_layer_->at(i).at(j).graphic_type;  //图像形式        //图层
        Scalar color = choose_color(graphic_layer_->at(i).at(j).color);  //颜色
        uint32_t start_angle =
            graphic_layer_->at(i).at(j).start_angle;  //圆弧开始角度
        uint32_t end_angle =
            graphic_layer_->at(i).at(j).end_angle;           //圆弧结束角度
        uint32_t width = graphic_layer_->at(i).at(j).width;  //宽度
        uint32_t start_x = graphic_layer_->at(i).at(j).start_x;  //起始坐标
        uint32_t start_y = graphic_layer_->at(i).at(j).start_y;  //起始坐标
        uint32_t radius = graphic_layer_->at(i).at(j).radius;    //半径
        uint32_t end_x = graphic_layer_->at(i).at(j).end_x;      //结束坐标x
        uint32_t end_y = graphic_layer_->at(i).at(j).end_y;      //结束坐标y
        switch (graphic_type) {
          case 0:
            break;
          case 1:  // zhixian
            line(radar_picture_show, Point(start_x, start_y), Point(end_x, end_y),
                 color, width);
            break;
          case 2:  // juxing
            rectangle(radar_picture_show, Rect(start_x, start_y, end_x, end_y),
                      color, 1, width, 0);
            break;
          case 3:  // yuan
            circle(radar_picture_show, Point(start_x, start_y), radius, color,
                   width);
            break;
          case 4:  // tuoyuan
          {
            int center_x = (start_x + end_x) / 2;
            int center_y = (start_y + end_y) / 2;
            int axes_x = abs(end_x - start_x) / 2;
            int axes_y = abs(end_y - start_y) / 2;
            ellipse(radar_picture_show, Point(center_x, center_y),
                    Size(axes_x, axes_y), 0, start_angle, end_angle, color,
                    width);
          } break;
          case 5:
            ellipse(radar_picture_show, Point(start_x, start_y),
                    Size(radius, radius), 0, start_angle, end_angle, color,
                    width);
            break;
          default:
            putText(radar_picture_show, graghic_name, Point(start_x, start_y),
                    FONT_HERSHEY_COMPLEX, 2, color, width);
        }
      }
    }
    imshow("radar_station", radar_picture_show);
#endif
  }
};
} // namespace minimap

} // namespace rm
extern rm::minimap::map *minimap;

#endif //RADAR_MINIMAP_H
