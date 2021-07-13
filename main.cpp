//
// Created by hoshino on 2021/4/2.
//
#include <glog/logging.h>
#include "src/mapping/include/3Dpnp.h"
#include "opencv2/opencv.hpp"
#include "armor_finder/armor_finder.h"
#include "rmconfig.h"
#include "rmtime.h"
//串口实例
// ArmorFinder实例
radar_pnp pnp_test;
ArmorFinder* armor_finder;
//运行时原图实例
RmConfig *config = new RmConfig();
RmTime *rmTime = new RmTime();
cv::Mat src;

void loop();
void init();


int main(int argc, char **argv) {
    init();
    loop();
    return 0;
}


void init()
{
    pnp_test.radar_debug_init();
    rmTime->init();
    config->init_from_file();
    armor_finder =
            new ArmorFinder(config->ENEMY_COLOR,config->ANTI_TOP);
}
void loop() {
    cv::Mat m0 = cv::imread("/home/hoshino/CLionProjects/radarpnp/src/101(1).bmp",1);
    cv::Mat m1;
    cv::resize(m0,m1,cv::Size(640,480));
    armor_finder->run(m1);
    std::vector<std::vector<cv::Point2d>> ord;
    std::vector<cv::Point2d> ord1={cv::Point2d(armor_finder->target_box.rect.x,
                                   armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),
                                   cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,
                                               armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),
                                   cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,
                                               armor_finder->target_box.rect.y),
                                   cv::Point2d(armor_finder->target_box.rect.x,
                                               armor_finder->target_box.rect.y)};
    ord.push_back(ord1);
    pnp_test.radar_solvePnp(ord);
}
/*
//cv::Mat m0 = cv::imread("/home/hoshino/CLionProjects/radar/src/mapping/src/101.bmp",1);
//    cv::Mat m1(656,492,CV_64F);
//    cv::imshow("image",m0);
//    cv::cvtColor(m0,m1,cv::COLOR_BGR2HSV);
//    cv::namedWindow("hsv");
//    int pos =0 ;
//    cv::Mat m21(656,492,CV_64F);
//    cv::Mat m22(656,492,CV_64F);
//    cv::inRange(m1,cv::Scalar(0,165,50),cv::Scalar(4,255,255),m21);
//    cv::inRange(m1,cv::Scalar(160,165,50),cv::Scalar(180,255,255),m22);
//    cv::Mat m3 = m21+m22;
//    cv::Mat m4;
//    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
//    cv::dilate(m3,m4,kernel);
//    cv::erode(m4,m3,kernel);
//    cv::erode(m3,m4,kernel);
//    std::vector<std::vector<cv::Point>> contours;
//    std::vector<cv::Vec4i> hi;
//    cv::findContours(m4,contours,hi,cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
//    cv::Mat m5(m21.size(),CV_8UC3);
//    std::vector<std::vector<cv::Point>> big2(contours.size());
//    for (int i = 0;i<contours.size();i++)
//    {
//        if(contours[i].size()>100)
//        {
////            cv::drawContours(m5,contours,i,cv::Scalar(255,0,0));
//            cv::Rect big;
//            big = cv::boundingRect(contours[i]);
//            cv::rectangle(m0,big,cv::Scalar(0,255,255));
//            std::vector<std::vector<cv::Point2d>> ord;
//            std::vector<cv::Point2d> ord1={cv::Point2d(armor_finder->target_box.rect.x,armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,armor_finder->target_box.rect.y),cv::Point2d(armor_finder->target_box.rect.x,armor_finder->target_box.rect.y)};
//            ord.push_back(ord1);
//            pnp_test.radar_solvePnp(ord);
//        }
//    }
//    cv::imshow("hsv",m0);
//
////    cv::createTrackbar("male","hsv",&pos,10,onchage,&m1);
//    cv::waitKey();void onchage(int pos,void* m1)
//{
//
//    cv::Mat m21(656,492,CV_64F);
//    cv::Mat m22(656,492,CV_64F);
//    cv::inRange(*(cv::Mat*)m1,cv::Scalar(0,165,50),cv::Scalar(4,255,255),m21);
//    cv::inRange(*(cv::Mat*)m1,cv::Scalar(160,165,50),cv::Scalar(180,255,255),m22);
//    cv::Mat m3 = m21+m22;
//    cv::Mat m4;
//    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
//    cv::dilate(m3,m4,kernel);
//    cv::erode(m4,m3,kernel);
//    cv::erode(m3,m4,kernel);
//    std::vector<std::vector<cv::Point>> contours;
//    std::vector<cv::Vec4i> hi;
//    cv::findContours(m4,contours,hi,cv::RETR_LIST,cv::CHAIN_APPROX_SIMPLE);
//    cv::Mat m5(m21.size(),CV_8UC3);
//    std::vector<std::vector<cv::Point>> big2(contours.size());
//    for (auto & contour : contours)
//    {
//        if(contour.size()>100)
//        {
////            cv::drawContours(m5,contours,i,cv::Scalar(255,0,0));
//            cv::Rect big;
//            big = cv::boundingRect(contour);
//            cv::rectangle(m5,big,cv::Scalar(0,255,255));
//            std::vector<std::vector<cv::Point2d>> ord;
//            std::vector<cv::Point2d> ord1={cv::Point2d(armor_finder->target_box.rect.x,armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,armor_finder->target_box.rect.y+armor_finder->target_box.rect.height),cv::Point2d(armor_finder->target_box.rect.x+armor_finder->target_box.rect.width,armor_finder->target_box.rect.y),cv::Point2d(armor_finder->target_box.rect.x,armor_finder->target_box.rect.y)};
//            ord.push_back(ord1);
//            pnp_test.radar_solvePnp(ord);
//        }
//    }
////    cv::imshow("hsv",m5);
//}
 */