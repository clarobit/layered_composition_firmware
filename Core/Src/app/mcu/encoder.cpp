#include "app/mcu/encoder.hpp"

namespace app::mcu {

Encoder::Encoder(TIM_HandleTypeDef *htim) : htim_(htim) {}

void Encoder::start() { HAL_TIM_Encoder_Start(htim_, TIM_CHANNEL_ALL); }

void Encoder::stop() { HAL_TIM_Encoder_Stop(htim_, TIM_CHANNEL_ALL); }

int32_t Encoder::getCount() const {
  return static_cast<int32_t>(__HAL_TIM_GET_COUNTER(htim_));
}

void Encoder::setCount(int32_t count) {
  __HAL_TIM_SET_COUNTER(htim_, static_cast<uint32_t>(count));
}

void Encoder::reset() { __HAL_TIM_SET_COUNTER(htim_, 0U); }

} // namespace app::mcu