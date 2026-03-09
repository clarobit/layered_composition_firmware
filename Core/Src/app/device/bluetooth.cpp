#include "app/device/bluetooth.hpp"

namespace app::device {

Bluetooth::Bluetooth(app::mcu::Uart uart, app::mcu::GpioOut power_pin,
                     app::mcu::GpioIn state_pin)
    : uart_(uart), power_pin_(power_pin), state_pin_(state_pin) {}

void Bluetooth::powerOn() { power_pin_.set(); }

void Bluetooth::powerOff() { power_pin_.reset(); }

bool Bluetooth::isConnected() const { return state_pin_.read(); }

HAL_StatusTypeDef Bluetooth::send(const uint8_t *data, uint16_t len,
                                  uint32_t timeout_ms) {
  return uart_.transmit(data, len, timeout_ms);
}

HAL_StatusTypeDef Bluetooth::receiveIt(uint8_t *data, uint16_t len) {
  return uart_.receiveIt(data, len);
}

} // namespace app::device