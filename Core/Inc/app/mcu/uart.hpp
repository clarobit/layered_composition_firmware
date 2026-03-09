#pragma once

#include <cstdint>

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"

namespace app::mcu {

class Uart {
public:
  explicit Uart(UART_HandleTypeDef *huart) : huart_(huart) {}

  HAL_StatusTypeDef transmit(const uint8_t *data, uint16_t len,
                             uint32_t timeout_ms = 1000U);

  HAL_StatusTypeDef receiveIt(uint8_t *data, uint16_t len);

  UART_HandleTypeDef *handle() const { return huart_; }

private:
  UART_HandleTypeDef *huart_;
};

} // namespace app::mcu