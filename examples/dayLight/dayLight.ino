/*
 * A third complex example of the use of the ScheduleTable library
 *
 * This example uses 3 ScheduleTable with 4 actions for the first one
 * and 1 action of the other two.
 *
 * The first schedule table manages the whole cyle with:
 * - a first action to start led dimming from 0 to 255
 * - a second action to start led dimming from 255 to 0
 * - a third action identical to the first one to start led dimming from 0 to 255
 * - a fourth action to start led dimming from 255 to 0 at faster pace
 *
 * The sketch is supposed to do the cycle in 24h but the execution is sped 1000 times
 * To change the speed, change the time base of each schedule table.
 */

#include <ScheduleTable.h>

/*
 * The Green LED cycles.
 */

/* 4 actions, period duration in seconds = 24h */
SchedTable<4> greenLEDCycle(86400);
/* 1 action, period duration = 7200 / 255 = 28 */
SchedTable<1> greenLEDPWMIncreaseCycle(28);
/* 1 action, period duration = 10800 / 255 = 42 */
SchedTable<1> greenLEDPWMDecreaseCycle(42); 

byte pwmGreenLED = 0;
byte pinGreenLED = 3;

/*
 * This function is called to start the increase of the PWM
 */
void startPWMIncrease()
{
  /* start the schedule table for 255 periods */
  greenLEDPWMIncreaseCycle.start(255);
}

/*
 * This function is called to start the decrease of the PWM
 */
void startPWMDecreaseFast()
{
  /* start the schedule table for 255 periods of 28s */
  greenLEDPWMDecreaseCycle.setPeriod(28);
  greenLEDPWMDecreaseCycle.start(255);
}

/*
 * This function is called to start the decrease of the PWM
 */
void startPWMDecreaseSlow()
{
  /* start the schedule table for 255 periods of 42s */
  greenLEDPWMDecreaseCycle.setPeriod(42);
  greenLEDPWMDecreaseCycle.start(255);
}

/*
 * This function is called to increase the PWM of the green LED
 */
void increaseGreenLEDPWM()
{
  pwmGreenLED++;
  analogWrite(pinGreenLED, pwmGreenLED);
}

/*
 * This function is called to decrease the PWM of the green LED
 */
void decreaseGreenLEDPWM()
{
  pwmGreenLED--;
  analogWrite(pinGreenLED, pwmGreenLED);
}

void setup() {
  pinMode(pinGreenLED,OUTPUT);
  pwmGreenLED = 0;
  analogWrite(pinGreenLED,pwmGreenLED);


  greenLEDCycle.at(27000,startPWMIncrease);     /* at 7:30 */
  greenLEDCycle.at(34200,startPWMDecreaseSlow); /* at 9:30 */
  greenLEDCycle.at(63000,startPWMIncrease);     /* at 17:30 */
  greenLEDCycle.at(75600,startPWMDecreaseFast); /* at 21:00 */

  greenLEDPWMIncreaseCycle.at(0,increaseGreenLEDPWM);
  greenLEDPWMDecreaseCycle.at(0,decreaseGreenLEDPWM);

  greenLEDCycle.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  ScheduleTable::update();
}
