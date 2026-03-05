#include "app/mcu/gpio.hpp"
#include "app/mcu/uart.hpp"
#include "main.h"

uint32_t test_main_cnt = 0;

void led_test();
static void uart_test(UART_HandleTypeDef *huart);

extern "C" void test_main(UART_HandleTypeDef *huart) {
  test_main_cnt++;

  // led_test();
  uart_test(huart);
}

// test 1 - LED
void led_test() {
  static app::mcu::GpioOut led(GPIOB, GPIO_PIN_0);

  led.toggle();
  HAL_Delay(1000);
}
// =====================================

// test 2 - UART
static app::mcu::Uart *uart = nullptr;

static void uart_test(UART_HandleTypeDef *huart) {
  static uint8_t rx;

  static bool started = false;

  if (uart == nullptr) {
    uart = new app::mcu::Uart(huart); // 처음 한 번만 주입
  }

  if (!started) {
    uart->receiveIt(&rx, 1);
    started = true;

    static const uint8_t msg[] = "uart rx test\r\n";
    uart->transmit(msg, sizeof(msg) - 1, 1000);
  }

  HAL_Delay(100);
}
// =====================================