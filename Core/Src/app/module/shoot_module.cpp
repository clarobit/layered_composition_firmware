#include "app/module/shoot_module.hpp"
#include <cstdlib>

namespace app::module {

ShootModule::ShootModule(TIM_HandleTypeDef *up_htim, uint32_t up_channel,
                         GPIO_TypeDef *up_dir_port, uint16_t up_dir_pin,
                         TIM_HandleTypeDef *down_htim, uint32_t down_channel,
                         GPIO_TypeDef *down_dir_port, uint16_t down_dir_pin)
    : up_motor_(up_htim, up_channel, up_dir_port, up_dir_pin),
      down_motor_(down_htim, down_channel, down_dir_port, down_dir_pin) {}

void ShootModule::start() {
  up_motor_.start();
  down_motor_.start();

  up_motor_.setDuty(0);
  down_motor_.setDuty(0);

  target_up_ = 0;
  target_down_ = 0;
  current_up_ = 0;
  current_down_ = 0;
  state_ = State::IDLE;
}

void ShootModule::stop() {
  up_motor_.stop();
  down_motor_.stop();

  target_up_ = 0;
  target_down_ = 0;
  current_up_ = 0;
  current_down_ = 0;
  state_ = State::IDLE;
}

void ShootModule::setTarget(int16_t up_duty, int16_t down_duty) {
  const int16_t up_max = static_cast<int16_t>(up_motor_.getMaxDuty());
  const int16_t down_max = static_cast<int16_t>(down_motor_.getMaxDuty());

  if (up_duty > up_max) {
    up_duty = up_max;
  } else if (up_duty < -up_max) {
    up_duty = -up_max;
  }

  if (down_duty > down_max) {
    down_duty = down_max;
  } else if (down_duty < -down_max) {
    down_duty = -down_max;
  }

  target_up_ = up_duty;
  target_down_ = down_duty;
  state_ = State::RAMPING;
}

void ShootModule::update() {
  if (state_ != State::RAMPING) {
    return;
  }

  const uint8_t up_done = updateMotor(up_motor_, target_up_, current_up_);
  const uint8_t down_done =
      updateMotor(down_motor_, target_down_, current_down_);

  if ((up_done != 0U) && (down_done != 0U)) {
    state_ = State::READY;
  }
}

uint8_t ShootModule::updateMotor(app::device::Motor &motor, int16_t target,
                                 int16_t &current) {
  const int16_t max_duty = static_cast<int16_t>(motor.getMaxDuty());

  if (target > max_duty) {
    target = max_duty;
  } else if (target < -max_duty) {
    target = -max_duty;
  }

  if (target > current + STEP) {
    current += STEP;
  } else if (target < current - STEP) {
    current -= STEP;
  } else {
    current = target;
  }

  if (current > 0) {
    motor.forward();
  } else if (current < 0) {
    motor.reverse();
  }

  motor.setDuty(static_cast<uint16_t>(std::abs(current)));

  return (current == target) ? 1U : 0U;
}

ShootModule::State ShootModule::getState() const { return state_; }

} // namespace app::module