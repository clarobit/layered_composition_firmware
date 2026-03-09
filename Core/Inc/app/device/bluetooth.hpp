#pragma once

#include <cstdint>

#include "app/mcu/gpio.hpp"
#include "app/mcu/uart.hpp"

namespace app::device {

class Bluetooth {
public:
  Bluetooth(app::mcu::Uart uart, app::mcu::GpioOut power_pin,
            app::mcu::GpioIn state_pin);

  void powerOn();
  void powerOff();

  bool isConnected() const;

  HAL_StatusTypeDef send(const uint8_t *data, uint16_t len,
                         uint32_t timeout_ms = 1000U);

  HAL_StatusTypeDef receiveIt(uint8_t *data, uint16_t len);

private:
  app::mcu::Uart uart_;
  app::mcu::GpioOut power_pin_;
  app::mcu::GpioIn state_pin_;
};

} // namespace app::device