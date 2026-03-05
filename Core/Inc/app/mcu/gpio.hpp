#pragma once

#include "stm32h7xx_hal.h"
#include <cstdint>

namespace app::mcu {

// Output GPIO
class GpioOut {
public:
  GpioOut(GPIO_TypeDef *port, uint16_t pin);

  void set();    // High
  void reset();  // Low
  void toggle(); // Toggle

private:
  GPIO_TypeDef *port_;
  uint16_t pin_;
};

// Input GPIO
class GpioIn {
public:
  GpioIn(GPIO_TypeDef *port, uint16_t pin);

  // true = High, false = Low
  bool read() const;

private:
  GPIO_TypeDef *port_;
  uint16_t pin_;
};

} // namespace app::mcu