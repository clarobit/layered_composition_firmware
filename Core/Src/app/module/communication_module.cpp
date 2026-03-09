#include "app/module/communication_module.hpp"

namespace app::module {

CommunicationModule::CommunicationModule(
    UART_HandleTypeDef *huart, GPIO_TypeDef *bt_power_port,
    uint16_t bt_power_pin, GPIO_TypeDef *bt_state_port, uint16_t bt_state_pin,
    ControlModule &control, ShootModule &shoot, FeederModule &feeder)
    : bluetooth_(app::mcu::Uart(huart),
                 app::mcu::GpioOut(bt_power_port, bt_power_pin),
                 app::mcu::GpioIn(bt_state_port, bt_state_pin)),
      control_(control), shoot_(shoot), feeder_(feeder) {}

void CommunicationModule::start() {
  bluetooth_.powerOn();

  rx_done_ = 0;
  drop_requested_ = 0;
  drop_reported_ = 0;
}

void CommunicationModule::stop() {
  bluetooth_.powerOff();

  rx_done_ = 0;
  drop_requested_ = 0;
  drop_reported_ = 0;
}

void CommunicationModule::update() {
  if (rx_done_ != 0U) {
    handleRx();
    rx_done_ = 0U;
  }

  handleFeeder();
}

uint8_t *CommunicationModule::getRxBuffer() { return rx_buf_; }

void CommunicationModule::setRxDone() { rx_done_ = 1U; }

void CommunicationModule::handleRx() {
  const int16_t x_target = parseInt16(&rx_buf_[0]);
  const int16_t y_target = parseInt16(&rx_buf_[2]);
  const int16_t up_target = parseInt16(&rx_buf_[4]);
  const int16_t down_target = parseInt16(&rx_buf_[6]);
  const uint8_t drop = rx_buf_[8];

  control_.setTarget(static_cast<int32_t>(x_target),
                     static_cast<int32_t>(y_target));
  shoot_.setTarget(up_target, down_target);

  if (drop == DROP_OFF) {
    feeder_.setState(FeederModule::State::IDLE);
    drop_requested_ = 0U;
    drop_reported_ = 0U;
  } else if (drop == DROP_ON) {
    drop_requested_ = 1U;
    drop_reported_ = 0U;
  }

  sendFrame(rx_buf_);
}

void CommunicationModule::handleFeeder() {
  if ((drop_requested_ != 0U) &&
      (control_.getState() == ControlModule::State::READY) &&
      (shoot_.getState() == ShootModule::State::READY) &&
      (feeder_.getState() == FeederModule::State::IDLE)) {
    feeder_.setState(FeederModule::State::RUNNING);
  }

  if ((feeder_.getState() == FeederModule::State::DONE) &&
      (drop_reported_ == 0U)) {
    sendDropDone();

    feeder_.setState(FeederModule::State::IDLE);
    drop_requested_ = 0U;
    drop_reported_ = 1U;
  }
}

void CommunicationModule::sendFrame(const uint8_t *data) {
  bluetooth_.send(data, FRAME_SIZE);
}

void CommunicationModule::sendDropDone() {
  const uint8_t data = DROP_DONE;
  bluetooth_.send(&data, 1);
}

int16_t CommunicationModule::parseInt16(const uint8_t *data) const {
  const uint16_t value =
      (static_cast<uint16_t>(data[0]) << 8) | static_cast<uint16_t>(data[1]);

  return static_cast<int16_t>(value);
}

} // namespace app::module