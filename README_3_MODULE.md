# Module Layer

Module Layer는 Device Layer를 조합하여 **기능 단위 로직을 구현하는 계층**.

이 계층에서는 HAL API를 직접 호출하지 않고
Motor, EncoderMotor, Switch, Bluetooth와 같은 Device를 조합하여
실제 동작 목적에 맞는 제어 로직을 구현함.

Module Layer는 단순한 장치 제어가 아니라
상태 기반 동작, 목표값 추적, 시퀀스 처리, 모듈 간 연동과 같은
상위 수준의 firmware logic을 담당함.

현재 프로젝트에서는 다음 4개의 Module을 구현함.

* ShootModule
* ControlModule
* FeederModule
* CommunicationModule

---

## ShootModule

ShootModule은 발사부의 상/하 모터 2개를 제어하는 Module.

두 개의 Motor device를 사용하며
외부에서 설정한 목표 duty까지 단계적으로 값을 변경하는 ramp control 방식으로 동작함.

양수 duty는 정방향, 음수 duty는 역방향을 의미하며
0은 정지 상태를 의미함.

---

### Constructor

```cpp
ShootModule(TIM_HandleTypeDef *up_htim, uint32_t up_channel,
GPIO_TypeDef *up_dir_port, uint16_t up_dir_pin,
TIM_HandleTypeDef *down_htim, uint32_t down_channel,
GPIO_TypeDef *down_dir_port, uint16_t down_dir_pin);
```

상단 모터와 하단 모터를 제어하기 위한 PWM Timer, Channel, 방향 제어 GPIO를 사용하는 객체를 생성함.

---

### State

```cpp
enum class State : uint8_t {
IDLE = 0,
RAMPING = 1,
READY = 2,
};
```

#### IDLE

정지 상태.

#### RAMPING

목표 duty까지 현재 duty를 단계적으로 증가 또는 감소시키는 상태.

#### READY

목표 duty에 도달한 상태.

---

### Functions

#### start

```cpp
void start();
```

두 개의 모터를 시작하고 duty를 0으로 초기화함.
현재 duty와 target duty도 모두 0으로 설정하며 state를 IDLE로 초기화함.

#### stop

```cpp
void stop();
```

두 개의 모터를 정지하고 state를 IDLE로 설정함.

#### setTarget

```cpp
void setTarget(int16_t up_duty, int16_t down_duty);
```

상단 모터와 하단 모터의 목표 duty를 설정함.

입력값은 각 모터의 최대 duty 범위를 초과하지 않도록 제한되며
target이 설정되면 state는 RAMPING으로 변경됨.

#### update

```cpp
void update();
```

현재 state가 RAMPING일 때 동작하며
각 모터의 현재 duty를 STEP 단위로 target 방향으로 이동시킴.

두 모터가 모두 target에 도달하면 state를 READY로 변경함.

#### getState

```cpp
State getState() const;
```

현재 ShootModule state를 반환함.

---

### Design

#### Signed duty

ShootModule 내부에서는 signed duty를 사용함.

```cpp

* value : forward

- value : reverse
  0       : stop
  ```

이를 통해 방향과 출력 크기를 하나의 값으로 표현할 수 있음.

#### Ramp control

목표 duty를 한 번에 변경하지 않고 STEP 단위로 점진적으로 변경함.

예:

```cpp
0 -> 30 -> 60 -> 90 -> ...
```

이를 통해 급격한 출력 변화를 줄이고
update 기반 상태 제어 구조를 만들 수 있음.

---

## ControlModule

ControlModule은 x축, y축의 위치 제어를 담당하는 Module.

두 개의 EncoderMotor와 두 개의 warning switch를 사용하며
외부에서 설정한 target count까지 모터를 이동시킴.

정확히 특정 count에서 정지하는 것은 어렵기 때문에
목표 count와 현재 count의 차이가 허용 오차 범위 안에 들어오면
목표 도달로 판단함.

현재 구현에서는 허용 오차를 50 count로 설정함.

또한 x축, y축 각각의 warning switch를 확인하여
위험 상태가 감지되면 전체 모터를 정지하고 warning state로 전환함.

---

### Constructor

```cpp
ControlModule(TIM_HandleTypeDef *x_pwm_htim, uint32_t x_pwm_channel,
GPIO_TypeDef *x_dir_port, uint16_t x_dir_pin,
TIM_HandleTypeDef *x_encoder_htim,
TIM_HandleTypeDef *y_pwm_htim, uint32_t y_pwm_channel,
GPIO_TypeDef *y_dir_port, uint16_t y_dir_pin,
TIM_HandleTypeDef *y_encoder_htim,
GPIO_TypeDef *x_warn_port, uint16_t x_warn_pin,
GPIO_TypeDef *y_warn_port, uint16_t y_warn_pin);
```

x축, y축 EncoderMotor와
각 축의 warning switch를 사용하는 객체를 생성함.

---

### State

```cpp
enum class State : uint8_t {
IDLE = 0,
MOVING = 1,
READY = 2,
WARNING_X = 3,
WARNING_Y = 4,
WARNING_XY = 5,
};
```

#### IDLE

정지 상태.

#### MOVING

x축, y축이 target count를 향해 이동 중인 상태.

#### READY

x축, y축이 모두 목표 위치에 도달한 상태.

#### WARNING_X

x축 warning switch가 눌린 상태.

#### WARNING_Y

y축 warning switch가 눌린 상태.

#### WARNING_XY

x축, y축 warning switch가 모두 눌린 상태.

---

### Functions

#### start

```cpp
void start();
```

x축, y축 EncoderMotor를 시작하고 duty를 0으로 초기화함.
현재 encoder count를 읽어 target count 초기값으로 사용하며 state를 IDLE로 설정함.

#### stop

```cpp
void stop();
```

x축, y축 모터 duty를 0으로 설정한 후 모터와 encoder를 정지하고 state를 IDLE로 설정함.

#### setTarget

```cpp
void setTarget(int32_t x_target_cnt, int32_t y_target_cnt);
```

x축, y축의 목표 count를 설정함.

warning switch가 눌린 상태가 아니라면 state를 MOVING으로 변경함.

#### update

```cpp
void update();
```

warning switch 상태를 먼저 확인함.
warning이 감지되면 전체 모터를 정지하고 warning state로 전환함.

warning 상태가 아니고 state가 MOVING이면
현재 encoder count를 읽어 target과 비교하고
오차가 허용 범위보다 크면 방향을 설정한 뒤 고정 duty로 이동시킴.

x축과 y축이 모두 목표 위치에 도달하면 state를 READY로 변경함.

#### getState

```cpp
State getState() const;
```

현재 ControlModule state를 반환함.

#### getXCount

```cpp
int32_t getXCount() const;
```

현재 x축 encoder count를 반환함.

#### getYCount

```cpp
int32_t getYCount() const;
```

현재 y축 encoder count를 반환함.

---

### Design

#### Position tolerance

정확히 target count에서 멈추기 어렵기 때문에
현재 count와 target count의 차이가 50 이내이면 목표 도달로 판단함.

예:

```cpp
target = 1000
current = 970 ~ 1030
-> READY
```

#### Fixed duty movement

현재 구현에서는 위치 오차가 존재할 때
고정 duty로 모터를 구동하여 target 방향으로 이동함.

가까운 거리에서 duty를 줄이는 정밀 제어는 아직 적용하지 않았으며
구조 단순성과 상태 기반 제어 흐름에 집중하여 구현함.

#### Warning handling

warning switch는 active-high 기준으로 사용하며
스위치가 눌리면 위험 상태로 판단함.

warning이 감지되면 해당 축만이 아니라 전체 동작을 정지시키고
state를 WARNING_X, WARNING_Y, WARNING_XY 중 하나로 변경함.

---

## FeederModule

FeederModule은 공 1개를 공급하는 역할을 담당하는 Module.

Motor 1개와 ball detect switch 1개를 사용하며
외부에서 state를 RUNNING으로 변경하면 모터를 구동함.

공이 떨어져 switch가 눌리면 feeding 완료로 판단하고
모터를 정지한 뒤 state를 DONE으로 변경함.

---

### Constructor

```cpp
FeederModule(TIM_HandleTypeDef *htim, uint32_t channel,
GPIO_TypeDef *dir_port, uint16_t dir_pin,
GPIO_TypeDef *sw_port, uint16_t sw_pin);
```

feeder motor와 ball detect switch를 사용하는 객체를 생성함.

---

### State

```cpp
enum class State : uint8_t {
IDLE = 0,
RUNNING = 1,
DONE = 2,
};
```

#### IDLE

대기 상태.
모터는 정지 상태를 유지함.

#### RUNNING

공급 동작이 진행 중인 상태.
모터를 정방향으로 구동함.

#### DONE

공 1개 공급이 완료된 상태.
모터를 정지한 상태를 유지함.

---

### Functions

#### start

```cpp
void start();
```

feeder motor를 시작하고 duty를 0으로 초기화한 뒤 state를 IDLE로 설정함.

#### stop

```cpp
void stop();
```

feeder motor duty를 0으로 설정한 뒤 모터를 정지하고 state를 IDLE로 설정함.

#### setState

```cpp
void setState(State state);
```

FeederModule state를 외부에서 설정함.

#### getState

```cpp
State getState() const;
```

현재 FeederModule state를 반환함.

#### update

```cpp
void update();
```

현재 state에 따라 feeder motor 동작을 수행함.

IDLE 상태에서는 duty를 0으로 유지함.
RUNNING 상태에서는 motor를 정방향으로 구동함.
switch가 눌리면 motor를 정지하고 state를 DONE으로 변경함.
DONE 상태에서는 motor를 정지 상태로 유지함.

---

### Design

#### State-driven operation

FeederModule은 별도의 target 값을 사용하지 않고
state 변화에 따라 동작하는 단순한 sequence module로 구성함.

즉 외부에서는 RUNNING 요청만 전달하고
실제 모터 구동은 update 내부의 state machine이 처리함.

#### One-shot feed

이 Module은 공 1개를 공급하는 one-shot 동작을 기준으로 구현함.

RUNNING 상태에서 switch가 눌리면
공이 실제로 떨어진 것으로 판단하고 DONE으로 전환함.

---

## CommunicationModule

CommunicationModule은 최상단에서 Bluetooth 통신을 처리하고
수신한 데이터를 각 Module에 전달하는 역할을 담당함.

Bluetooth device를 사용하여 9바이트 고정 길이 frame을 수신하고
frame 안에 포함된 x, y, shoot up, shoot down, drop 명령을 해석하여
ControlModule, ShootModule, FeederModule을 제어함.

또한 데이터를 수신하면 동일한 9바이트 데이터를 그대로 응답하고
공이 실제로 떨어졌을 때는 0xFF 1바이트를 추가로 송신함.

---

### Constructor

```cpp
CommunicationModule(UART_HandleTypeDef *huart, GPIO_TypeDef *bt_power_port,
uint16_t bt_power_pin, GPIO_TypeDef *bt_state_port,
uint16_t bt_state_pin, ControlModule &control,
ShootModule &shoot, FeederModule &feeder);
```

Bluetooth 통신에 필요한 UART, 전원 GPIO, 연결 상태 GPIO와
ControlModule, ShootModule, FeederModule 참조를 사용하는 객체를 생성함.

---

### Communication Frame

고정 길이 9바이트 frame을 사용함.

```cpp
[0] [1] : x target
[2] [3] : y target
[4] [5] : shoot up target
[6] [7] : shoot down target
[8]     : drop
```

각 2바이트 target 값은 int16_t로 해석하며
현재 구현에서는 high byte 먼저 사용하는 방식으로 처리함.

drop 값은 다음 의미를 가짐.

```cpp
0 : feeder off
1 : feeder request
```

공이 실제로 떨어졌을 때는 9바이트 frame이 아니라

```cpp
0xFF
```

1바이트를 송신하여 drop 완료를 알림.

---

### Functions

#### start

```cpp
void start();
```

Bluetooth 전원을 켜고 내부 통신 상태를 초기화함.

#### stop

```cpp
void stop();
```

Bluetooth 전원을 끄고 내부 통신 상태를 초기화함.

#### update

```cpp
void update();
```

수신 완료 플래그를 확인하여 새 frame이 들어오면 handleRx를 실행함.
이후 feeder 동작 조건을 확인하는 handleFeeder를 실행함.

#### getRxBuffer

```cpp
uint8_t *getRxBuffer();
```

UART interrupt 수신에 사용할 9바이트 RX buffer 주소를 반환함.

#### setRxDone

```cpp
void setRxDone();
```

UART 수신 완료 callback에서 호출되며
새로운 9바이트 frame이 수신되었음을 표시함.

---

### Design

#### Fixed-size frame

프로토콜을 단순하게 유지하기 위해
시작 바이트, 종료 바이트, escape 처리 없이
항상 9바이트 고정 frame만 사용함.

이 방식은 구현과 디버깅이 단순하고
포트폴리오 프로젝트에서 전체 구조 설명에 집중하기에 적합함.

#### Echo response

새 frame을 수신하면 수신한 9바이트를 그대로 한 번 송신함.

이를 통해 통신이 정상적으로 연결되었는지
상대 기기에서 즉시 확인할 수 있음.

#### Module orchestration

CommunicationModule은 각 Module의 update를 직접 수행하지 않음.

update loop는 main.c에서 다음 순서로 수행함.

```cpp
control_update();
shoot_update();
feeder_update();
communication_update();
```

CommunicationModule은 이 구조 위에서
수신한 명령을 각 Module에 전달하고
ControlModule과 ShootModule이 READY 상태가 되면
FeederModule을 RUNNING으로 전환하는 orchestration 역할을 담당함.

#### Drop complete report

FeederModule이 DONE 상태가 되면
CommunicationModule은 0xFF 1바이트를 송신하여
공이 실제로 떨어졌음을 외부에 알림.

---

## Module Layer Summary

현재 Module Layer는 다음과 같은 역할 분리를 가짐.

* ShootModule
  발사 모터 속도 제어

* ControlModule
  x, y 위치 제어

* FeederModule
  공 1개 공급

* CommunicationModule
  Bluetooth 통신 및 상위 orchestration

이 구조를 통해

* Device Layer는 장치 단위 abstraction
* Module Layer는 기능 단위 logic
* main.c는 loop와 callback만 담당

하도록 계층을 분리함.

즉 이 프로젝트의 Module Layer는
단순한 device wrapper가 아니라
상태 기반 firmware logic을 구현하는 핵심 계층으로 설계됨.
