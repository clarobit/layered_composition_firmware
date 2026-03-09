// ===== MCU Layer =====
#include "app/mcu/encoder.hpp"
#include "app/mcu/gpio.hpp"
#include "app/mcu/pwm.hpp"
#include "app/mcu/uart.hpp"

// ===== HAL / System =====
#include "main.h"

// ===== HAL Handles =====
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;

// ===== Test State =====
uint32_t test_main_cnt = 0;
int32_t test_value = 0;

// ===== MCU Test Functions =====
static void encoder_test();
static void gpio_test();
static void pwm_test();
static void uart_test();

// =====================================
// test entry
extern "C" void test_main(void) {

  test_main_cnt++;

  // ===== MCU Tests =====
  // encoder_test();
  // gpio_test();
  // pwm_test();
  // uart_test();
}
// =====================================

// =====================================
// MCU test 1 - Encoder
static void encoder_test() {
  static app::mcu::Encoder encoder(&htim4);
  static bool started = false;

  if (!started) {
    encoder.start();
    started = true;
  }

  test_value = encoder.getCount();
}
// =====================================

// =====================================
// MCU test 2 - GPIO
static void gpio_test() {
  static app::mcu::GpioOut led(GPIOB, GPIO_PIN_0);
  static app::mcu::GpioIn input(GPIOB, GPIO_PIN_2);

  static bool state = false;

  state = !state;

  if (state) {
    led.set();
  } else {
    led.reset();
  }

  test_value = input.read();
}
// =====================================

// =====================================
// MCU test 3 - PWM
static void pwm_test() {
  static app::mcu::Pwm pwm(&htim3, TIM_CHANNEL_1);
  static bool started = false;
  static int duty = 0;
  static int step = 50;

  if (!started) {
    pwm.start();
    started = true;
  }

  duty += step;
  if (duty >= pwm.getMaxDuty() || duty <= 0) {
    step = -step;
  }

  pwm.setDuty(duty);
  test_value = pwm.getDuty();
}
// =====================================

// =====================================
// MCU test 4 - UART
uint8_t uart_rx_buf_test[20];
uint32_t uart_rx_index = 0;

static void uart_test() {
  static app::mcu::Uart uart(&huart2);
  static bool started = false;

  if (!started) {
    uart.receiveIt(&uart_rx_buf_test[uart_rx_index], 1);
    started = true;
  }

  const uint8_t msg[] = "uart test start\r\n";
  uart.transmit(msg, sizeof(msg) - 1);

  test_value = uart_rx_buf_test[0]; // 디버거 확인용
}
// =====================================