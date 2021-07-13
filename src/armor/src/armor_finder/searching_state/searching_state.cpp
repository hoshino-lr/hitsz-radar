//
// Created by xinyang on 19-3-27.
//

#include <armor_finder/armor_finder.h>
#include <rmconfig.h>
#include <show_images/show_images.h>

bool ArmorFinder::stateSearchingTarget(cv::Mat &src) {
    if (findArmorBox(src, target_box)) {return true;}
    else {  //串口没输出的话看这里
        for (auto & it : target_box)
        {
            it = ArmorBox();
        }
        return false;
    }
}
