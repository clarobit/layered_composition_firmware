#include "app/device/motor.hpp"

namespace app::device {

Motor::Motor(TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *dir_port,
             uint16_t dir_pin)
    : pwm_(htim, channel), dir_(dir_port, dir_pin) {}

void Motor::start() { pwm_.start(); }

void Motor::stop() {
  pwm_.setDuty(0);
  pwm_.stop();
}

void Motor::forward() { dir_.set(); }

void Motor::reverse() { dir_.reset(); }

void Motor::setDuty(uint16_t duty) {
  const uint16_t max_duty = pwm_.getMaxDuty();

  if (duty > max_duty) {
    duty = max_duty;
  }

  pwm_.setDuty(duty);
}

uint16_t Motor::getMaxDuty() const { return pwm_.getMaxDuty(); }

} // namespace app::device