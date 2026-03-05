#include "app/mcu/pwm.hpp"

namespace app::mcu {

Pwm::Pwm(TIM_HandleTypeDef *htim, uint32_t channel)
    : htim_(htim), channel_(channel) {}

void Pwm::start() { HAL_TIM_PWM_Start(htim_, channel_); }

void Pwm::stop() { HAL_TIM_PWM_Stop(htim_, channel_); }

void Pwm::setDuty(uint16_t duty) {
  __HAL_TIM_SET_COMPARE(htim_, channel_, duty);
}

uint16_t Pwm::getMaxDuty() const {
  return static_cast<uint16_t>(__HAL_TIM_GET_AUTORELOAD(htim_));
}

} // namespace app::mcu