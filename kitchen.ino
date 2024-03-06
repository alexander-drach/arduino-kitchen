#include <GyverStepper.h>
#include "GyverButton.h"

GButton stopDownSensor(2);
GButton stopUpSensor(5);

GButton slowDownSpeed(3);
GButton slowUpSpeed(4);

GStepper<STEPPER2WIRE> stepper(200, 11, 10);

int speed = 400;
int slowSpeed = 200;

bool motionDetected = false;
bool firstMotionDetected = false;

bool moving = false;

bool handSensor = false;

bool statePositionDown = false;
bool statePositionUp = false;

int direct = 0;

void setup() {
  Serial.begin(9600);

  stepper.setRunMode(KEEP_SPEED);

  pinMode(7, OUTPUT);
}

void loop() {
  stopDownSensor.tick();
  stopUpSensor.tick();

  slowDownSpeed.tick();
  slowUpSpeed.tick();  

  stepper.tick();

  motionDetected = !digitalRead(12); // сенсор движения фартука
  handSensor = digitalRead(6); // датчик сувания руки

  if (handSensor) {
    stepper.brake();
    direct = 0;
  }

  if (stopDownSensor.isPress()) { // остановка при касании датчика внизу
    stepper.brake();
    direct = 0;
    digitalWrite(7, HIGH); // снятие напряжения без библиотеки
    moving = false;
    statePositionDown = false;
    statePositionUp = false;
  }

  if (stopUpSensor.isPress()) { // остановка при касании датчика вверху
    stepper.brake();
    direct = 0;
    digitalWrite(7, HIGH); // снятие напряжения без библиотеки
    moving = false;
    statePositionDown = false;
    statePositionUp = false;
  }

  if (motionDetected) { // условие - счетчик для паузы
    direct++;
    delay(500);

    if (direct > 2) {
      direct = 1;
    }
  }

  if (motionDetected && !handSensor) { // если коснулись датчик движения и не сработал датчик движения руки, выполняем разные действия ГЛАВНОЕ УСЛОВИЕ
    digitalWrite(7, LOW);  

    if (!firstMotionDetected) { // старт при первом запуке - едет наверх
      firstMotionDetected = true;
      stepper.setSpeed(speed);
      moving = true;
      statePositionDown = true;
      statePositionUp = false;
    }

    if (firstMotionDetected && (!stopDownSensor.isHold() || !stopUpSensor.isHold())) { // старт при следующих запусках из неизвестного положения - едет наверх
      stepper.setSpeed(speed);
      moving = true;
      statePositionDown = true;
      statePositionUp = false;
    }

    if (stopDownSensor.isHold() && motionDetected) { // старт из нижнего положения при нажатом нижнем датчике
      // Serial.println("edem vverh");
      stepper.setSpeed(speed);
      moving = true;
      statePositionDown = true;
      statePositionUp = false;
    }

    if (stopUpSensor.isHold() && motionDetected) { // старт из верхнего положения при нажатом верхнем датчике 
      // Serial.println("edem vniz");
      stepper.setSpeed(-speed);
      moving = true;
      statePositionUp = true;
      statePositionDown = false;
    }

    if (direct == 2) { // остановка при касании датчика если уже едет - пауза
      stepper.brake();
      moving = false;
    }
  }
    
  if (slowUpSpeed.isPress() && statePositionDown) { // замедление скорости вверху ДАТЧИК - 2
    // Serial.println("up sensor slow");
    stepper.setSpeed(slowSpeed);
  }
    
  if (slowDownSpeed.isPress() && statePositionUp) { // замедление скорости внизу ДАТЧИК - 1
    // Serial.println("down sensor slow");  
    stepper.setSpeed(-slowSpeed);
  }
  // если не работаем с датчиком, то работаем с замедлениями
}