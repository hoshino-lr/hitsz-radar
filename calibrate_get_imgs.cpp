#include <glog/logging.h>
#include <rmconfig.h>
#include <rmcamera.h>
#include <opencv2/opencv.hpp>
#include <string>

int img_cnt = 40;
rm::config::general *config = new rm::config::general;
auto* camera = new rm::camera;

int main(int argc, char** argv) {
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr = true;
    google::InitGoogleLogging(argv[0]);
    config->init_from_file();
    camera->open(&config->camConfig1,config->ARMOR_CAMERA_EXPOSURE,config->ARMOR_CAMERA_GAIN);
    cv::Mat SRC(640, 480, CV_8UC3);
    int cnt = 0;
    while (camera->is_open()) {
        camera->get_frame(SRC);
        cv::resize(SRC,SRC,cv::Size(640,480));
        cv::imshow("origin", SRC);
        int key = cv::waitKey(20);
        if (key == 's') {
            cv::imwrite("../cam_calibrate/" + std::to_string(cnt++) + ".jpg",
                        SRC);
            LOG(INFO) << cnt;
            if (cnt == img_cnt) {
                break;
            }
        }
    }
    return 0;
}
