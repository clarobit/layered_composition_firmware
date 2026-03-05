#pragma once

#include "main.h"
#include <cstdint>

namespace app::mcu {

class Encoder {
public:
  explicit Encoder(TIM_HandleTypeDef* htim);

  void start();
  void stop();

  int32_t getCount() const;
  void setCount(int32_t count);
  void reset();

private:
  TIM_HandleTypeDef* htim_;
};

} // namespace app::mcu