#include "app/module/feeder_module.hpp"

namespace app::module {

FeederModule::FeederModule(TIM_HandleTypeDef *htim, uint32_t channel,
                           GPIO_TypeDef *dir_port, uint16_t dir_pin,
                           GPIO_TypeDef *sw_port, uint16_t sw_pin)
    : motor_(htim, channel, dir_port, dir_pin), ball_sw_(sw_port, sw_pin) {}

void FeederModule::start() {
  motor_.start();
  motor_.setDuty(0);
  state_ = State::IDLE;
}

void FeederModule::stop() {
  motor_.setDuty(0);
  motor_.stop();
  state_ = State::IDLE;
}

void FeederModule::setState(State state) { state_ = state; }
FeederModule::State FeederModule::getState() const { return state_; }

void FeederModule::update() {

  switch (state_) {

  case State::IDLE:
    motor_.setDuty(0);
    break;

  case State::RUNNING:
    motor_.forward();
    motor_.setDuty(FEED_DUTY);

    if (ball_sw_.isPressed()) {
      motor_.setDuty(0);
      state_ = State::DONE;
    }
    break;

  case State::DONE:
    motor_.setDuty(0);
    break;
  }
}

} // namespace app::module