//// 本算法是图像拼接算法，能自动搜寻兴趣点
//
//#include <iostream>
//#include <vector>
//#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/features2d.hpp>
//#include <opencv2/calib3d.hpp>
//#include <opencv2/xfeatures2d.hpp>
//#include <opencv2/stitching.hpp>
//
//int main() {
//  // Read input images
//  cv::Mat image1 = cv::imread("radar.png", 0);
//  cv::Mat image2 = cv::imread("radar_.png", 0);
//  if (!image1.data || !image2.data)
//    return 0;
//
//  // Display the images
//  cv::namedWindow("Image 1");
//  cv::imshow("Image 1", image1);
//  cv::namedWindow("Image 2");
//  cv::imshow("Image 2", image2);
//
//  // vector of keypoints and descriptors
//  std::vector<cv::KeyPoint> keypoints1;
//  std::vector<cv::KeyPoint> keypoints2;
//  cv::Mat descriptors1, descriptors2;
//
//  // 1. Construction of the SIFT feature detector
//  cv::Ptr<cv::Feature2D> ptrFeature2D = cv::SIFT::create(74);
//
//  // 2. Detection of the SIFT features and associated descriptors
//  ptrFeature2D->detectAndCompute(image1, cv::noArray(), keypoints1, descriptors1);
//  ptrFeature2D->detectAndCompute(image2, cv::noArray(), keypoints2, descriptors2);
//
//  std::cout << "Number of feature points (1): " << keypoints1.size() << std::endl;
//  std::cout << "Number of feature points (2): " << keypoints2.size() << std::endl;
//
//  // 3. Match the two image descriptors
//
//  // Construction of the matcher with crosscheck
//  cv::BFMatcher matcher(cv::NORM_L2, true);
//  // matching
//  std::vector<cv::DMatch> matches;
//  matcher.match(descriptors1, descriptors2, matches);
//
//  // draw the matches
//  cv::Mat imageMatches;
//  cv::drawMatches(image1, keypoints1,  // 1st image and its keypoints
//                  image2, keypoints2,  // 2nd image and its keypoints
//                  matches,      // the matches
//                  imageMatches,    // the image produced
//                  cv::Scalar(255, 255, 255),  // color of the lines
//                  cv::Scalar(255, 255, 255),  // color of the keypoints
//                  std::vector<char>(),
//                  static_cast<cv::DrawMatchesFlags>(2));
//  cv::namedWindow("Matches (pure rotation case)");
//  cv::imshow("Matches (pure rotation case)", imageMatches);
//
//  // Convert keypoints into Point2f
//  std::vector<cv::Point2f> points1, points2;
//  for (auto matche : matches) {
//
//    // Get the position of left keypoints
//    float x = keypoints1[matche.queryIdx].pt.x;
//    float y = keypoints1[matche.queryIdx].pt.y;
//    points1.emplace_back(x, y);
//    // Get the position of right keypoints
//    x = keypoints2[matche.trainIdx].pt.x;
//    y = keypoints2[matche.trainIdx].pt.y;
//    points2.emplace_back(x, y);
//  }
//
//  std::cout << points1.size() << " " << points2.size() << std::endl;
//
//  // Find the homography between image 1 and image 2
//  std::vector<char> inliers;
//  cv::Mat homography = cv::findHomography(
//          points1, points2, // corresponding points
//          inliers,       // outputed inliers matches 输出的局内匹配项
//          cv::RANSAC,       // RANSAC method
//          1.);           // max distance to reprojection point
//
//  // Draw the inlier points
//  cv::drawMatches(image1, keypoints1,  // 1st image and its keypoints
//                  image2, keypoints2,  // 2nd image and its keypoints
//                  matches,      // the matches
//                  imageMatches,    // the image produced
//                  cv::Scalar(255, 255, 255),  // color of the lines
//                  cv::Scalar(255, 255, 255),  // color of the keypoints
//                  inliers,
//                  static_cast<cv::DrawMatchesFlags>(2));
//  cv::namedWindow("Homography inlier points");
//  cv::imshow("Homography inlier points", imageMatches);
//
//
////    在计算单应矩阵后,就可以把一幅图像的点转移到另
////    一幅图像上。实际上,图像中的每个像素都可以转移,因此可以把整幅图像迁移到另一幅图像的
////    的视点上。这个过程称为图像拼接,常用于将多幅图像构建成一幅大型全景图。
//  // 将第一幅图像扭曲到第二幅图像
//  cv::Mat result;
//  cv::warpPerspective(image1, // input image
//                      result,      // output image
//                      homography,    // homography
//                      cv::Size(2 * image1.cols, image1.rows)); // size of output image
//
//  // Copy image 1 on the first half of full image
//  // 把第一幅图像复制到完整图像的第一个半边
//  cv::Mat half(result, cv::Rect(0, 0, image2.cols, image2.rows));
//  image2.copyTo(half);// 把 image2 复制到 image1 的 ROI 区域
//
//  // Display the warp image
//  cv::namedWindow("Image mosaic");
//  cv::imshow("Image mosaic", result);
//
//  // Read input images
//  std::vector<cv::Mat> images;
//  images.push_back(cv::imread("radar.png"));
//  images.push_back(cv::imread("radar_.png"));
//
//  cv::Mat panorama; // output panorama输出的全景图
//  // create the stitcher 创建拼接器
//  std::shared_ptr<cv::Stitcher> stitcher = cv::Stitcher::create();//createDefault();
//  // stitch the images 拼接图像
//  cv::Stitcher::Status status = stitcher->stitch(images, panorama);
//
//  if (status == cv::Stitcher::OK) // success?
//  {
//    // Display the panorama
//    cv::namedWindow("Panorama");
//    cv::imshow("Panorama", panorama);
//  }
//
//  cv::waitKey();
//  return 0;
//}



#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;
const char *source_window = "Source image";
const char *warp_window = "Warp";
const char *warp_rotate_window = "Warp + Rotate";

int main(int, char **argv) {
  Point2f srcTri[3];
  Point2f dstTri[3];
  Mat rot_mat(2, 3, CV_32FC1);
  Mat warp_mat(2, 3, CV_32FC1);
  Mat src, warp_dst, warp_rotate_dst;
  src = imread("radar.png", IMREAD_COLOR);
  warp_dst = Mat::zeros(src.rows, src.cols, src.type());
  srcTri[0] = Point2f(0, 0);
  srcTri[1] = Point2f(src.cols - 1.f, 0);
  srcTri[2] = Point2f(0, src.rows - 1.f);
  dstTri[0] = Point2f(src.cols * 0.0f, src.rows * 0.33f);
  dstTri[1] = Point2f(src.cols * 0.85f, src.rows * 0.25f);
  dstTri[2] = Point2f(src.cols * 0.15f, src.rows * 0.7f);
  warp_mat = getAffineTransform(srcTri, dstTri);
  warpAffine(src, warp_dst, warp_mat, warp_dst.size());
  Point center = Point(warp_dst.cols / 2, warp_dst.rows / 2);
  double angle = -50.0;
  double scale = 0.6;
  rot_mat = getRotationMatrix2D(center, angle, scale);
  warpAffine(warp_dst, warp_rotate_dst, rot_mat, warp_dst.size());
  namedWindow(source_window, WINDOW_AUTOSIZE);
  imshow(source_window, src);
  namedWindow(warp_window, WINDOW_AUTOSIZE);
  imshow(warp_window, warp_dst);
  namedWindow(warp_rotate_window, WINDOW_AUTOSIZE);
  imshow(warp_rotate_window, warp_rotate_dst);
  waitKey(0);
  return 0;
}