#pragma once

#include "app/device/bluetooth.hpp"
#include "app/module/control_module.hpp"
#include "app/module/feeder_module.hpp"
#include "app/module/shoot_module.hpp"
#include "main.h"

#include <cstdint>

namespace app::module {

class CommunicationModule {
public:
  CommunicationModule(UART_HandleTypeDef *huart, GPIO_TypeDef *bt_power_port,
                      uint16_t bt_power_pin, GPIO_TypeDef *bt_state_port,
                      uint16_t bt_state_pin, ControlModule &control,
                      ShootModule &shoot, FeederModule &feeder);

  void start();
  void stop();

  void update();

  uint8_t *getRxBuffer();
  void setRxDone();

private:
  static constexpr uint8_t FRAME_SIZE = 9;
  static constexpr uint8_t DROP_OFF = 0;
  static constexpr uint8_t DROP_ON = 1;
  static constexpr uint8_t DROP_DONE = 0xFF;

  app::device::Bluetooth bluetooth_;

  ControlModule &control_;
  ShootModule &shoot_;
  FeederModule &feeder_;

  uint8_t rx_buf_[FRAME_SIZE] = {0};

  uint8_t rx_done_ = 0;
  uint8_t drop_requested_ = 0;
  uint8_t drop_reported_ = 0;

  void handleRx();
  void handleFeeder();
  void sendFrame(const uint8_t *data);
  void sendDropDone();
  int16_t parseInt16(const uint8_t *data) const;
};

} // namespace app::module