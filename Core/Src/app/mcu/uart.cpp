#include "app/mcu/uart.hpp"

namespace app::mcu {

HAL_StatusTypeDef Uart::transmit(const uint8_t *data, uint16_t len,
                                 uint32_t timeout_ms) {
  return HAL_UART_Transmit(huart_, const_cast<uint8_t *>(data), len,
                           timeout_ms);
}

HAL_StatusTypeDef Uart::transmitIt(const uint8_t *data, uint16_t len) {
  return HAL_UART_Transmit_IT(huart_, const_cast<uint8_t *>(data), len);
}

HAL_StatusTypeDef Uart::receiveIt(uint8_t *data, uint16_t len) {
  return HAL_UART_Receive_IT(huart_, data, len);
}

} // namespace app::mcu