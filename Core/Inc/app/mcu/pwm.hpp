#pragma once

#include "main.h"
#include <cstdint>

namespace app::mcu {

class Pwm {
public:
  Pwm(TIM_HandleTypeDef *htim, uint32_t channel);

  void start();
  void stop();

  void setDuty(uint16_t duty);
  uint16_t getMaxDuty() const;

private:
  TIM_HandleTypeDef *htim_;
  uint32_t channel_;
};

} // namespace app::mcu