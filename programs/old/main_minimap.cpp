/*
 * @desp: 标定程序测试
 * 测试已经标定好的数据
 * */
#include <radar.hpp>
#include <rmcamera.h>
#include <utils.h>
#include <minimap.h>
#include <mapping.h>


int main(int argc, char **argv) {
  FLAGS_alsologtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Welcome to mini-map!";

  rm::minimap::map map = rm::minimap::map();
  map.team_us = rm::minimap::team_type::red;
  uint16_t robot_hero_id = map.robot_get_id(rm::minimap::team_type::us,
                                            rm::minimap::object_type::hero);
  map.robot_add(robot_hero_id, rm::minimap::object(rm::minimap::object_type::hero,
                                               rm::minimap::team_type::us, "己方英雄"));

  uint16_t robot_hero_id_2 = map.robot_get_id(rm::minimap::team_type::enemy,
                                              rm::minimap::object_type::hero);
  map.robot_add(robot_hero_id_2, rm::minimap::object(rm::minimap::object_type::hero,
                                                 rm::minimap::team_type::enemy, "敌方英雄"));

  uint64_t running = true;
  std::default_random_engine rand_e;
  std::uniform_int_distribution<int> rand_u_width(10, rm::minimap::map_width - 10);
  std::uniform_int_distribution<int> rand_u_height(10, rm::minimap::map_height - 10);

  map.robot_update(rm::minimap::map::update_operate::credibility, robot_hero_id_2,
                   rm::minimap::credibility_type::fuzzy);
  map.robot_update(rm::minimap::map::update_operate::credibility, robot_hero_id,
                   rm::minimap::credibility_type::disappeared);

  while (running++ < 0xFFFFFFFF) {
    map.robot_update(rm::minimap::map::update_operate::position, robot_hero_id,
                     std::vector<int>{rand_u_width(rand_e), rand_u_height(rand_e)});
    map.robot_update(rm::minimap::map::update_operate::position, robot_hero_id_2,
                     std::vector<int>{rand_u_width(rand_e), rand_u_height(rand_e)});
    // 随机更新可信度
    map.robot_update(rm::minimap::map::update_operate::credibility, robot_hero_id_2,
                     (rm::minimap::credibility_type) ((rand_u_width(rand_e)) % 3));
    map.draw(1000);
  }

  LOG(INFO) << "Done!";
  return 0;
}