#pragma once

#include "app/mcu/gpio.hpp"
#include "app/mcu/pwm.hpp"
#include "main.h"
#include <cstdint>

namespace app::device {

class Motor {
public:
  Motor(TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *dir_port,
        uint16_t dir_pin);

  void start();
  void stop();

  void forward();
  void reverse();

  void setDuty(uint16_t duty);
  uint16_t getMaxDuty() const;

private:
  app::mcu::Pwm pwm_;
  app::mcu::GpioOut dir_;
};

} // namespace app::device