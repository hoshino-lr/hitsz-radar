#include <armor_finder/armor_finder.h>
#include <rmconfig.h>
ArmorBox::ArmorBox(const cv::Rect &pos, const LightBlobs &blobs, uint8_t color,
                   int i)
    : rect(pos), light_blobs(blobs), box_color(color), id(i) {
};



//获取实际的装甲板从右下开始逆时针的四个边界点
std::vector<cv::Point2f> ArmorBox::getArmorPoints() const {
    if(light_blobs.size() != 2){
        auto o  = rect.tl();
        float w = rect.width,h = rect.height;
        return {cv::Point2f(o.x,o.y + h),o,cv::Point2f(o.x + w,o.y),cv::Point2f(o.x + w,o.y + h)};
    }
    cv::Point2f points[4],tmp[4];
    auto rectL = light_blobs.at(0).rect;
    auto rectR = light_blobs.at(1).rect;
    if(rectL.center.x > rectR.center.x) std::swap(rectL,rectR);
    if(rectL.size.width > rectL.size.height){
        //rectL.size.width *= 2;
        rectL.points(tmp);
        points[0] = tmp[1];
        points[1] = tmp[2];
    }
    else{
        //rectL.size.height *= 2;
        rectL.points(tmp);
        points[0] = tmp[0];
        points[1] = tmp[1];
    }

    if(rectR.size.width > rectR.size.height){
        //rectR.size.width *= 2;
        rectR.points(tmp);
        points[2] = tmp[3];
        points[3] = tmp[0];
    }
    else{
        //rectR.size.height *= 2;
        rectR.points(tmp);
        points[2] = tmp[2];
        points[3] = tmp[3];
    }
    return {points[0],points[1],points[2],points[3]};
}

// 获取装甲板中心点
cv::Point2f ArmorBox::getCenter() const {
    return cv::Point2f(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

// 获取两个灯条中心点的间距
double ArmorBox::getBlobsDistance() const {
    if (light_blobs.size() == 2) {
        auto &x = light_blobs[0].rect.center;
        auto &y = light_blobs[1].rect.center;
        return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y));
    } else {
        return 0;
    }
}

// 获取灯条长度和间距的比例
double ArmorBox::lengthDistanceRatio() const {
    if (light_blobs.size() == 2) {
        return std::max(light_blobs[0].length, light_blobs[1].length) /
               getBlobsDistance();
    } else {
        return 100;
    }
}

// 计算装甲板到摄像头的距离
double ArmorBox::getBoxDistance() {
//    auto ans = armorSolvePnP();
//    return ans.second.z;
return 0;
}

// 装甲板的优先级比较
bool ArmorBox::operator<(const ArmorBox &box) const {
    if (id != box.id) {
        if (box_color == BOX_BLUE) {
            return prior_blue[id2name[id]] < prior_blue[id2name[box.id]];
        } else {
            return prior_red[id2name[id]] < prior_red[id2name[box.id]];
        }
    } else {
        auto d1 = (rect.x - config->IMAGE_CENTER_X) * (rect.x - config->IMAGE_CENTER_X) +
                  (rect.y - config->IMAGE_CENTER_Y) * (rect.y - config->IMAGE_CENTER_Y);
        auto d2 =
            (box.rect.x - config->IMAGE_CENTER_X) * (box.rect.x - config->IMAGE_CENTER_X) +
            (box.rect.y - config->IMAGE_CENTER_Y) * (box.rect.y - config->IMAGE_CENTER_Y);
        return d1 < d2;
    }
}
