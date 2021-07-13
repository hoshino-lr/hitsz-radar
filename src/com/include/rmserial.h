#ifndef H_RMSERIAL
#define H_RMSERIAL

#include <radar.hpp>
#include <serial/serial.h>

#include <mutex>

struct EnemyConfig {
  uint16_t pridict_x;
  uint16_t pridict_y;
  uint8_t type;
};

typedef struct _EnemyData {
  uint8_t start_flag; //
  uint16_t current_x; //
  uint16_t current_y; //
  uint16_t distance;  //
  uint16_t angle;     //
  uint8_t type;       //
  uint8_t self_type;  //
  uint8_t end_flag;   //
} EnemyData;


class RmSerial {
public:
  serial::Serial *active_port;

  ~RmSerial();

  bool init_success;
  bool thread_running;

  bool init();

#ifndef RADAR_USE_FAKE_TTL
  bool isConnected() const { return active_port->isOpen(); }
#else

  static bool isConnected() { return true; }

#endif

  bool send_data(uint8_t *data, size_t size);

  void start_thread();

  void stop_thread();
};

extern std::mutex receive_mtx;
extern EnemyConfig receive_config_data;
extern RmSerial *rmSerial;
#endif