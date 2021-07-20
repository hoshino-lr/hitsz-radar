//
// Created by xinyang on 19-3-27.
//

#ifndef _ARMOR_FINDER_H_
#define _ARMOR_FINDER_H_

#include <armor_finder/armor_box.h>
#include <armor_finder/light_blobs.h>
#include <rmconfig.h>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>
#include <3Dpnp.h>
#include <RoundQueue.h>
#define BLOB_RED 1
#define BLOB_BLUE 0
#define BOX_RED 1
#define BOX_BLUE 0
void point_dl(std::vector<cv::Point2d> boxes1,std::vector<cv::Point2d> boxes2,cv::Mat map);
extern std::map<int, std::string> id2name;  //装甲板id到名称的map
extern std::map<std::string, int> name2id;  //装甲板名称到id的map
extern std::map<std::string, int> prior_blue;
extern std::map<std::string, int> prior_red;

typedef struct
{
  RoundQueue<cv::Point2d,4> filtering;
}Point_filtering;
/********************* 自瞄类定义 **********************/
class ArmorFinder {
   public:
    explicit ArmorFinder(const int &color,const int &Id);
    ~ArmorFinder() = default;
    std::vector<ArmorBox> target_box = std::vector<ArmorBox>(2,ArmorBox());
private:
    typedef enum {
        SEARCHING_STATE,
        TRACKING_STATE,
        STANDBY_STATE
    } State;  // 自瞄状态枚举定义
    Radarpnp radar_pnp;
    int id;
    std::vector<cv::Rect> roi =  std::vector<cv::Rect>(2,cv::Rect());
    double frame_time{};       // 当前帧对应时间
    const int &enemy_color;  // 敌方颜色，引用外部变量，自动变化
    State state;             // 自瞄状态对象实例
    std::vector<ArmorBox> last_box = std::vector<ArmorBox>(2,ArmorBox());  // 目标装甲板
    std::vector<Point_filtering> points = std::vector<Point_filtering>(2,Point_filtering());
    bool findLightBlobs(const cv::Mat &src, LightBlobs &light_blobs);
    bool findArmorBox(const cv::Mat &src, std::vector<ArmorBox> &box);
    bool matchArmorBoxes(const cv::Mat &src, const LightBlobs &light_blobs,
                         ArmorBoxes &armor_boxes);
    bool stateSearchingTarget(cv::Mat &src);  // searching state主函数
    bool sendBoxPosition(uint16_t shoot, double dist = -1);  // 和主控板通讯
    std::vector<cv::Point2d> wave_fliter(std::vector<cv::Point2d> now_points);
    cv::Point2d get_average(int num);
   public:
    std::vector<cv::Point2d> run(cv::Mat &src,cv::Mat & map);  // 自瞄主函数
    float getPointLength(cv::Point_<float> point);
};

#endif /* _ARMOR_FINDER_H_ */
