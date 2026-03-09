# Application Layer (app_main)

Application Layer는 전체 Firmware의 **최상위 실행 계층**.

이 계층은 STM32Cube에서 생성되는 `main.c`와
C++ 기반 Module Layer 사이의 **C ↔ C++ bridge 역할**을 수행함.

STM32CubeIDE는 기본적으로 C 기반 프로젝트 구조를 사용하기 때문에
C++로 구현된 Module 객체를 직접 `main.c`에서 사용하기 어렵다.

이를 해결하기 위해 `app_main.cpp`를 통해

* Module 객체 생성
* Module 초기화
* Module update 호출
* UART callback과 연결되는 interface 제공

을 담당하도록 구성함.

Application Layer는 실제 기능 로직을 구현하지 않고
**Module Layer를 실행하고 연결하는 역할만 수행함.**

---

## File Structure

Application Layer는 다음 두 파일로 구성됨.

```cpp
app/app_main.hpp
app/app_main.cpp
```

`app_main.hpp`는 C와 C++ 모두에서 사용할 수 있도록 작성된 header이며
`app_main.cpp`는 실제 Module 객체 생성과 실행을 담당함.

---

## C and C++ Bridge

STM32Cube에서 생성되는 `main.c`는 C 코드이므로
C++ 클래스 객체를 직접 사용할 수 없음.

따라서 Application Layer에서는
`extern "C"` 인터페이스를 사용하여 C에서 호출 가능한 함수들을 제공함.

예:

```cpp
#ifdef __cplusplus
extern "C" {
#endif

void app_init(void);
void control_update(void);
void shoot_update(void);
void feeder_update(void);
void communication_update(void);

uint8_t *comm_get_rx_buffer(void);
void comm_set_rx_done(void);

#ifdef __cplusplus
}
#endif
```

이 함수를 통해 `main.c`는 내부 구현을 알 필요 없이
Application Layer의 함수만 호출하여 전체 시스템을 실행할 수 있음.

---

## Module Object Creation

`app_main.cpp`에서는 모든 Module 객체를 생성함.

```cpp
namespace {

app::module::ControlModule control(...);

app::module::ShootModule shoot(...);

app::module::FeederModule feeder(...);

app::module::CommunicationModule communication(
    &huart2,
bt_power_port,
bt_power_pin,
bt_state_port,
bt_state_pin,
control,
shoot,
feeder);

}
```

Module 객체는 namespace 내부 static 영역에 생성하여
프로그램 전체에서 하나의 instance만 사용하도록 구성함.

또한 CommunicationModule은
ControlModule, ShootModule, FeederModule을 참조로 받아
각 Module을 제어할 수 있도록 설계됨.

---

## Initialization

Application Layer는 `app_init()` 함수를 통해
모든 Module을 초기화함.

```cpp
void app_init(void);
```

`main.c`에서는 peripheral 초기화 이후
이 함수를 호출하여 전체 Module을 시작함.

예:

```cpp
app_init();
HAL_UART_Receive_IT(&huart2, comm_get_rx_buffer(), 9);
```

이 과정에서

* ControlModule start
* ShootModule start
* FeederModule start
* CommunicationModule start

가 수행됨.

---

## Update Loop

Firmware의 main loop에서는
각 Module의 update 함수를 주기적으로 호출함.

```cpp
control_update();
shoot_update();
feeder_update();
communication_update();
```

이 구조는 **update 기반 state machine 구조**를 사용하며
각 Module은 내부 상태에 따라 동작을 수행함.

현재 프로젝트에서는 약 10ms 주기로 update가 호출되도록 설계됨.

---

## UART Callback Interface

Bluetooth 통신은 UART interrupt를 통해 데이터를 수신함.

UART 수신 완료 callback에서는
CommunicationModule에 수신 완료를 전달함.

`main.c`

```cpp
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
if (huart->Instance == USART2)
{
comm_set_rx_done();
HAL_UART_Receive_IT(&huart2, comm_get_rx_buffer(), 9);
}
}
```

여기서

* `comm_get_rx_buffer()`
  CommunicationModule 내부 RX buffer 주소 반환

* `comm_set_rx_done()`
  새 frame이 수신되었음을 CommunicationModule에 전달

이 구조를 통해 interrupt에서는 최소한의 작업만 수행하고
실제 데이터 처리 로직은 `communication.update()`에서 실행됨.

---

## Interrupt and Main Loop Separation

Application Layer에서는 다음과 같은 구조를 사용함.

Interrupt context

```cpp
UART receive interrupt
→ flag set
```

Main loop

```cpp
communication.update()
→ handleRx()
→ module control
```

즉 interrupt에서는 **데이터 수신 완료만 표시하고**
실제 처리 로직은 main loop에서 수행하도록 설계함.

이 방식은 다음 장점을 가짐.

* interrupt 실행 시간 최소화
* 시스템 안정성 향상
* 디버깅 용이성

---

## Overall Firmware Flow

전체 firmware 실행 흐름은 다음과 같음.

```cpp
main.c
↓
app_init()

main loop
↓
control_update()
shoot_update()
feeder_update()
communication_update()

UART interrupt
↓
comm_set_rx_done()
```

Application Layer는 이 구조에서
**main.c와 Module Layer를 연결하는 역할**을 수행함.

---

## Application Layer Summary

Application Layer는 다음 역할을 수행함.

* C 기반 main.c와 C++ Module Layer 연결
* Module 객체 생성 및 초기화
* main loop에서 Module update 실행
* UART interrupt와 CommunicationModule 연결

이를 통해

* main.c는 최소한의 코드만 유지
* Module Layer는 순수 C++ 로직으로 구성
* firmware 구조의 계층 분리 유지

가 가능하도록 설계됨.

Application Layer는 전체 firmware 구조에서
**Module Layer를 실행하는 entry point 역할**을 담당함.
