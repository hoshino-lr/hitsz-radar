#include <mapping.h>

//初始化函数
void Mapping::init() {
  //建树与构建映射 （以后考虑从文件读入）
  buildKdTree(camera_position_tree, camera_position, 0);
  for (unsigned i = 0; i < camera_position.size(); i++) {
    camera2map.insert(std::pair<std::vector<int>, std::vector<int> >(camera_position[i], minimap_position[i]));
  }
  //初始化机器人信息
  //如果只识别敌方机器人的话，颜色好像没太大用
  for (int i = 0; i < ROBOTAMOUNT; i++) {
    robot[i].last_x = 0;
    robot[i].current_x = 0;
    robot[i].last_y = 0;
    robot[i].current_y = 0;
    robot[i].state = VANISH;
    robot[i].robotype = i + 1;
    robot[i].velocity = 0;
    robot[i].angle = 0;
  }
  //初始化时间
  rmTime->init();
  current_time = rmTime->seconds();
}

//刷新位置与状态
void Mapping::refreshRobotInfo() {
  for (int i = 0; i < ROBOTAMOUNT; i++) {
    robot[i].last_x = robot[i].current_x;
    robot[i].last_y = robot[i].current_y;
    robot[i].state = VANISH;
  }
}

//更新机器人信息
void Mapping::updateRobotInfo(std::vector<RoboInputinfo> cars) {
  refreshRobotInfo();

  std::vector<int> current_carcamera_position;
  std::vector<int> current_nearest_camera_position;
  std::vector<int> current_carminimap_position;

  int current_cars_amount = cars.size();

  for (int i = 0; i < current_cars_amount; i++) {
    //最近点匹配与坐标映射
    current_carcamera_position.clear();
    current_carcamera_position.push_back(cars[i].x);
    current_carcamera_position.push_back(cars[i].y);
    current_nearest_camera_position = searchNearestNeighbor(current_carcamera_position, camera_position_tree);
    std::map<std::vector<int>, std::vector<int> >::iterator iter;
    iter = camera2map.find(current_nearest_camera_position);
    current_carminimap_position.assign(iter->second.begin(), iter->second.end());
    //将结果写入机器人信息中
    robot[cars[i].robotype - 1].current_x = current_carminimap_position[0];
    robot[cars[i].robotype - 1].current_y = current_carminimap_position[1];
    robot[cars[i].robotype - 1].state = EXIST;
  }
}

//计算机器人速度、方向和对state为VANISH的目标位置做出预测，预测方式为沿着消失前的切线方向按原速度直行
void Mapping::calculateAndPredict() {
  last_time = current_time;
  current_time = rmTime->seconds();
  double delta_time = current_time - last_time;
  for (auto & i : robot) {
    if (i.state == EXIST)     //机器人在视野内时计算速度与朝向
    {
      i.velocity = sqrt(pow(i.current_x - i.last_x, 2) +
                               pow(i.current_y - i.last_y, 2)) / delta_time * MINIMAP_SCALE;
      if (i.current_x == i.last_x)
        i.angle = atan((i.current_y - i.last_y) / (i.current_x - i.last_x + 1));
      else i.angle = atan((i.current_y - i.last_y) / (i.current_x - i.last_x));

    } else                            //机器人在视野外时预测位置
    {
      i.current_x = (int)((double)i.velocity * cos(i.angle) * delta_time / MINIMAP_SCALE);
      i.current_y = (int)((double)i.velocity * sin(i.angle) * delta_time / MINIMAP_SCALE);
    }

  }
}
Mapping *mapping;
MappingTriangle *mapping2;
