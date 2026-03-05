#include "app/mcu/gpio.hpp"

namespace app::mcu {

// ===== GpioOut =====
GpioOut::GpioOut(GPIO_TypeDef *port, uint16_t pin) : port_(port), pin_(pin) {}

void GpioOut::set() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }

void GpioOut::reset() { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }

void GpioOut::toggle() { HAL_GPIO_TogglePin(port_, pin_); }

// ===== GpioIn =====
GpioIn::GpioIn(GPIO_TypeDef *port, uint16_t pin) : port_(port), pin_(pin) {}

bool GpioIn::read() const {
  return (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET);
}

} // namespace app::mcu