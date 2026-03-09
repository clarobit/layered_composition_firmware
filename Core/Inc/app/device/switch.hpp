#pragma once

#include "app/mcu/gpio.hpp"
#include "main.h"
#include <cstdint>

namespace app::device {

class Switch {
public:
  Switch(GPIO_TypeDef *port, uint16_t pin);

  bool isPressed() const;

private:
  app::mcu::GpioIn gpio_;
};

} // namespace app::device