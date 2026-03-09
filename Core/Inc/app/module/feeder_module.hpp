#pragma once

#include "app/device/motor.hpp"
#include "app/device/switch.hpp"
#include "main.h"

#include <cstdint>

namespace app::module {

class FeederModule {
public:
  enum class State : uint8_t {
    IDLE = 0,
    RUNNING = 1,
    DONE = 2,
  };

  FeederModule(TIM_HandleTypeDef *htim, uint32_t channel,
               GPIO_TypeDef *dir_port, uint16_t dir_pin, GPIO_TypeDef *sw_port,
               uint16_t sw_pin);

  void start();
  void stop();

  void setState(State state);
  State getState() const;
  void update();

private:
  app::device::Motor motor_;
  app::device::Switch ball_sw_;

  State state_ = State::IDLE;

  static constexpr uint16_t FEED_DUTY = 300;
};

} // namespace app::module