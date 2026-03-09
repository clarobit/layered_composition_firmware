#pragma once

#include "app/device/motor.hpp"
#include "main.h"

#include <cstdint>

namespace app::module {

class ShootModule {
public:
  enum class State : uint8_t {
    IDLE = 0,
    RAMPING = 1,
    READY = 2,
  };

  ShootModule(TIM_HandleTypeDef *up_htim, uint32_t up_channel,
              GPIO_TypeDef *up_dir_port, uint16_t up_dir_pin,
              TIM_HandleTypeDef *down_htim, uint32_t down_channel,
              GPIO_TypeDef *down_dir_port, uint16_t down_dir_pin);

  void start();
  void stop();

  void setTarget(int16_t up_duty, int16_t down_duty);
  void update();

  State getState() const;

private:
  app::device::Motor up_motor_;
  app::device::Motor down_motor_;

  int16_t target_up_ = 0;
  int16_t target_down_ = 0;

  int16_t current_up_ = 0;
  int16_t current_down_ = 0;

  State state_ = State::IDLE;

  static constexpr int16_t STEP = 30;

  uint8_t updateMotor(app::device::Motor &motor, int16_t target,
                      int16_t &current);
};

} // namespace app::module