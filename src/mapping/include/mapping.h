#ifndef RADAR_H
#define RADAR_H

#include <radar.hpp>
#include <utility>
#include <rmutils.h>
#include <memory>
#include <minimap.h>

#define VANISH 0            //机器人存在状态
#define EXIST 1
//#define ROBOTAMOUNT 12      //可以识别到机器人的总个数
#define ROBOTAMOUNT 6      //对方机器人个数
#define MINIMAP_SCALE 10000 //小地图比例尺，需要计算一下

//传入的机器人数据结构体
struct RoboInputinfo {
  int x;
  int y;      //坐标
  int color;  //颜色
  int robotype;    //机器人兵种
};

//机器人信息
//此处x，y坐标为小地图下坐标，因小地图中的速度没有实际意义，故此处速度为乘比例尺后的实际速度
struct RobotInfo {
  int last_x;
  int last_y;
  int current_x;
  int current_y;
  int color;              //红为0，蓝为1
  int robotype;
  double velocity;        //速度
  double angle;           //速度与x正方向夹角
  int state;              //存在状态，exist和vanish，状态为vanish时对位置做出预测
};


class KdTree {
public:
  std::vector<int> root;
  KdTree *parent;
  KdTree *leftChild;
  KdTree *rightChild;

  //构造函数
  KdTree();

  //判断kd树为空
  bool isEmpty();

  //判断是否为叶节点
  bool isLeaf();

  //判断是否为根节点
  bool isRoot();

  //判断是否为父节点的左节点
  bool isLeft();

  //判断是否为父节点的右节点
  bool isRight();
};

std::vector<std::vector<int> > Transpose(std::vector<std::vector<int> > Matrix);

double measureDistance(std::vector<int> point1, std::vector<int> point2);

int findMiddleValue(std::vector<int> vec);

//构建kd树
void buildKdTree(KdTree *tree, std::vector<std::vector<int> > data, unsigned depth);

//在kd树tree中搜索目标点goal的最近邻
//输入：目标点；已构造的kd树
//输出：目标点的最近邻
std::vector<int> searchNearestNeighbor(std::vector<int> goal, KdTree *tree);

//逐层打印kd树
void printKdTree(KdTree *tree, unsigned depth);


class Mapping {
public:

  std::vector<std::vector<int> > camera_position;       //保存的图像坐标
  std::vector<std::vector<int> > minimap_position;      //保存的小地图坐标
  std::map<std::vector<int>, std::vector<int> > camera2map;

  double current_time{};
  double last_time{};

  RobotInfo robot[ROBOTAMOUNT]{};               //0-11代表敌方1-12号机器人
  KdTree *camera_position_tree = new KdTree;
  rm::utils::rm_time *rmTime = new rm::utils::rm_time;

  //初始化函数
  void init();

  //构造函数
  Mapping() = default;

  void refreshRobotInfo();

  void updateRobotInfo(std::vector<RoboInputinfo> cars);

  void calculateAndPredict();
};

extern Mapping *mapping;

// 新的Mapping方法

class Triangle {
public:
  std::vector<cv::Point> pts;
  // 额外记录对应的小地图点
  std::vector<cv::Point> mpts;

  Triangle(const cv::Point &_pt1, const cv::Point &_pt2, const cv::Point &_pt3) :
          pts({_pt1, _pt2, _pt3}) {}

  Triangle(const cv::Point &_pt1, const cv::Point &_pt2, const cv::Point &_pt3,
           const cv::Point &_mpt1, const cv::Point &_mpt2, const cv::Point &_mpt3) :
          pts({_pt1, _pt2, _pt3}), mpts({_mpt1, _mpt2, _mpt3}) {}
};


class MappingTriangle {
public:
  std::map<std::vector<int>, std::vector<int>> camera2map;
  std::vector<Triangle> triangles;
  cv::Size size;
  cv::Mat calibration;
  cv::Mat calibration2;
  cv::Mat calibration_map;

  MappingTriangle() = default;

  explicit MappingTriangle(cv::Size size_) : size(std::move(size_)) {}

  MappingTriangle(int width, int height) : size(cv::Size(width, height)) {}

  static std::vector<int> point2Vector(const cv::Point &pt) { return std::vector<int>({pt.x, pt.y}); }

  static cv::Point vector2Point(std::vector<int> p) { return {p[0], p[1]}; }

  static bool is_same_line(std::vector<cv::Point> pts) {
    if (pts.size() != 3) return true;
    if (pts[0].y == pts[1].y) return pts[2].y == pts[1].y;
    if (pts[2].y == pts[1].y) return pts[0].y == pts[1].y;
    return (pts[0].x - pts[1].x) / (pts[0].y - pts[1].y) == (pts[2].x - pts[1].x) / (pts[2].y - pts[1].y);
  }

  void delaunayAlgorithm() {
    cv::Rect boundRc(0, 0, this->size.width, this->size.height);
    if (camera2map.empty()) {
      return;
    }
    // 转换到points
    std::vector<cv::Point> points;
    for (const auto &p : camera2map)
      points.emplace_back(cv::Point(p.first[0], p.first[1]));

    std::vector<Triangle> result;

    std::vector<cv::Vec6f> _temp_result;
    cv::Subdiv2D subdiv2d(boundRc);
    for (const auto &point : points) {
      subdiv2d.insert(cv::Point2f((float) point.x, (float) point.y));
    }
    subdiv2d.getTriangleList(_temp_result);
    for (const auto &_tmp_vec : _temp_result) {
      cv::Point pt1((int) _tmp_vec[0], (int) _tmp_vec[1]);
      cv::Point pt2((int) _tmp_vec[2], (int) _tmp_vec[3]);
      cv::Point pt3((int) _tmp_vec[4], (int) _tmp_vec[5]);
      result.emplace_back(pt1, pt2, pt3);
    }
    // 更新triangles的对应点
    // map的key是唯一的
    triangles.clear();
    for (auto tri : result) {
      std::vector<int> pt_vec1 = point2Vector(tri.pts[0]),
              pt_vec2 = point2Vector(tri.pts[1]),
              pt_vec3 = point2Vector(tri.pts[2]);
      if (camera2map.find(pt_vec1) != camera2map.end() &&
          camera2map.find(pt_vec2) != camera2map.end() &&
          camera2map.find(pt_vec3) != camera2map.end()) {
        Triangle trii = Triangle(tri.pts[0], tri.pts[1], tri.pts[2],
                                 vector2Point(camera2map[pt_vec1]),
                                 vector2Point(camera2map[pt_vec2]),
                                 vector2Point(camera2map[pt_vec3]));
        if (is_same_line(trii.pts) || is_same_line(trii.mpts)) continue;
        triangles.emplace_back(trii);
      }
    }
  }

  // 格式转换
  void init(const std::vector<std::pair<std::vector<int>, std::vector<int>>> &camera2map_vec) {
    std::map<std::vector<int>, std::vector<int>> camera2map_map;
    for (const auto &p : camera2map_vec)
      camera2map_map.insert(p);
    init(camera2map_map);
  }

  // 初始化、绘图
  void init(const std::map<std::vector<int>, std::vector<int>> &camera2map_map) {
    this->camera2map = camera2map_map;
    // 生成三角形和对应
    this->delaunayAlgorithm();
    // 绘图
    // 生成大一点...uint16型，最大支持65535个三角形
//    calibration.create(this->size, CV_16UC1);
    calibration = cv::Mat::zeros(this->size, CV_16UC1);
//    calibration2.create(this->size, CV_8UC3);
    calibration2 = cv::Mat::zeros(this->size, CV_8UC3);
//    calibration_map.create(cv::Size(rm::minimap::map_width, rm::minimap::map_height), CV_8UC3);
    calibration_map = cv::Mat::zeros(cv::Size(rm::minimap::map_width, rm::minimap::map_height), CV_8UC3);
    for (uint16_t i = 0; i < this->triangles.size(); i++) {
      cv::fillPoly(calibration, triangles[i].pts, cv::Scalar_<uint16_t>(i));
      cv::fillPoly(calibration2, triangles[i].pts,
                   cv::Scalar(i * 255 / this->triangles.size(), 127,
                              i * 255 / this->triangles.size()));
      cv::fillPoly(calibration_map, triangles[i].mpts,
                   cv::Scalar(i * 255 / this->triangles.size(), 127,
                              i * 255 / this->triangles.size()));

    }
  }

  uint16_t get_color(cv::Point pt) { return calibration.at<uint16_t>(std::move(pt)); }

  static std::vector<float> make_quadrilateral(const std::vector<cv::Point> &pts) {
    std::vector<cv::Point> result;
    // 选择左边的点做顶点
    cv::Point select = pts[0];
    for (const auto &p : pts)
      if (p.x < select.x)
        select = p;
    cv::Point d;
    if (select == pts[0]) {
      d = pts[1] + pts[2] - select;
      result = {select, pts[1], d, pts[2]};
    } else if (select == pts[1]) {
      d = pts[0] + pts[2] - select;
      result = {pts[0], select, pts[2], d};
    } else {  // select == pts[2]
      d = pts[1] + pts[0] - select;
      result = {pts[0], d, pts[1], pts[2]};
    }
    std::vector<float> r;
    for (const auto &p : result) {
      r.emplace_back((float) p.x);
      r.emplace_back((float) p.y);
    }
    return r;
  }

  cv::Point get_mapping_point(const cv::Point &pt) {
    uint16_t color = get_color(pt);
    if (color >= triangles.size()) return {0, 0};
    Triangle tri = triangles[color];
    // 做个放射变换
    // 需要四个点，就用三角形最右边的边扩展出平行四边形
    // 注意，录入数据、处理数据的时候必须注意不能共线
//    auto qpts = make_quadrilateral(tri.pts), mqpts = make_quadrilateral(tri.mpts);
//    float qpts_data[8], mqpts_data[8];
//    for (int i = 0; i < 8; i++) qpts_data[i] = qpts[i], mqpts_data[i] = mqpts[i];
    /*
    // 拿到仿射变换矩阵
    cv::Mat trans = cv::getPerspectiveTransform(cv::Mat(4, 2, CV_32F, &qpts_data), cv::Mat(4, 2, CV_32F, &mqpts_data));
//    trans.convertTo(trans, CV_16U);
    // shape of trans: 3x3
    LOG(INFO) << "shape of trans: " << trans.size << ", type=" << trans.type();
    LOG(INFO) << "trans: " << trans;
    cv::Mat origin(1, 3, CV_64F);
    origin.at<double>(0, 0) = (double) pt.x;
    origin.at<double>(0, 1) = (double) pt.y;
    origin.at<double>(0, 2) = (double) 1;
//    LOG(INFO) << "shape of origin: " << origin.size << ", type=" << origin.type();
    cv::Mat result = origin * trans;
    LOG(INFO) << "result: " << result;
    if (result.at<double>(0, 2) == 0) return {0, 0};
//    return cv::Point((int)(result.at<double>(0, 0) / result.at<double>(0, 2)),
//                     (int)(result.at<double>(0, 1) / result.at<double>(0, 2)));
    return {(int) (result.at<double>(0, 0)),
            (int) (result.at<double>(0, 1))};
    */
    cv::Point2f p1s[3], p2s[3];
    auto p1p = p1s, p2p = p2s;
    for (int i = 0; i < 3; i++) {
      p1p->x = tri.pts[i].x, p1p->y = tri.pts[i].y;
      p1p++;
    }
    for (int i = 0; i < 3; i++) {
      p2p->x = tri.mpts[i].x, p2p->y = tri.mpts[i].y;
      p2p++;
    }
//    p1s->x = tri.pts[0].x, p1s->x = tri.pts[0].x
    auto M = cv::getAffineTransform(p1s, p2s);
//    LOG(INFO) << "shape of M: " << M.size << ", type=" << M.type();
//    LOG(INFO) << "M: " << M;
    cv::Mat origin(3, 1, CV_64F);
    origin.at<double>(0, 0) = (double) pt.x;
    origin.at<double>(1, 0) = (double) pt.y;
    origin.at<double>(2, 0) = (double) 1;
//    LOG(INFO) << "shape of origin: " << origin.size << ", type=" << origin.type();
//    LOG(INFO) << "origin: " << origin;
    cv::Mat result = M * origin;
//    LOG(INFO) << "shape of result: " << result.size << ", type=" << result.type();
//    LOG(INFO) << "result: " << result;
//    LOG(INFO) << "return: " << cv::Point{
//    (int) result.at<double>(0, 0), (int) result.at<double>(1, 0)
//  };
  return { (int) result.at<double>(0, 0), (int) result.at<double>(1, 0) };
}

};

extern MappingTriangle *mapping2;

#endif  // RADAR_H
