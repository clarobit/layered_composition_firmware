#include "app/app_main.hpp"

#include "app/module/communication_module.hpp"
#include "app/module/control_module.hpp"
#include "app/module/feeder_module.hpp"
#include "app/module/shoot_module.hpp"

#include "main.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

namespace {

app::module::ControlModule control(&htim3, TIM_CHANNEL_1, GPIOB, GPIO_PIN_0,
                                   &htim4, &htim3, TIM_CHANNEL_2, GPIOB,
                                   GPIO_PIN_1, &htim4, GPIOB, GPIO_PIN_2, GPIOB,
                                   GPIO_PIN_3);

app::module::ShootModule shoot(&htim3, TIM_CHANNEL_3, GPIOB, GPIO_PIN_4, &htim3,
                               TIM_CHANNEL_4, GPIOB, GPIO_PIN_5);

app::module::FeederModule feeder(&htim3, TIM_CHANNEL_1, GPIOB, GPIO_PIN_6,
                                 GPIOB, GPIO_PIN_7);

app::module::CommunicationModule communication(&huart2, GPIOB, GPIO_PIN_8,
                                               GPIOB, GPIO_PIN_9, control,
                                               shoot, feeder);
#include "app/app_main.hpp"
} // namespace

extern "C" {

void app_init(void) {
  control.start();
  shoot.start();
  feeder.start();
  communication.start();
}

void control_update(void) { control.update(); }

void shoot_update(void) { shoot.update(); }

void feeder_update(void) { feeder.update(); }

void communication_update(void) { communication.update(); }

uint8_t *comm_get_rx_buffer(void) { return communication.getRxBuffer(); }

void comm_set_rx_done(void) { communication.setRxDone(); }
}