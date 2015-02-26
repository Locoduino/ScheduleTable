/*
 * More complex example of the use of the ScheduleTable library
 *
 * This example uses 2 ScheduleTable with 3 action each. 
 * Each schedule table manage a traffic light.
 * 
 * Each traffic light has a 30s period.
 * - Green light lasts for 13s
 * - Yellow light lasts for 2s
 * - Red light lasts for 15s
 * Schedule tables are shifted so that one light switches from
 * red to green when the other one shift from yellow to red.
 */

#include <ScheduleTable.h>
 
ScheduleTable firstLightCycle(3,30,1000);
ScheduleTable secondLightCycle(3,30,1000);

struct trafficLight {
  byte greenPin;
  byte yellowPin;
  byte redPin;
};

const struct trafficLight firstLight  = { 3, 4, 5 };
const struct trafficLight secondLight = { 6, 7, 8 };

enum { GREEN, YELLOW, RED };

void setLightState(const struct trafficLight& light, const byte state)
{
  digitalWrite(light.greenPin, LOW);
  digitalWrite(light.yellowPin, LOW);
  digitalWrite(light.redPin, LOW);
  switch (state) {
    case GREEN:  digitalWrite(light.greenPin, HIGH);  break;
    case YELLOW: digitalWrite(light.yellowPin, HIGH); break;
    case RED:    digitalWrite(light.redPin, HIGH);    break;
  }
}

void firstLightGreen()   { setLightState(firstLight, GREEN);   }
void firstLightYellow()  { setLightState(firstLight, YELLOW);  }
void firstLightRed()     { setLightState(firstLight, RED);     }
void secondLightGreen()  { setLightState(secondLight, GREEN);  }
void secondLightYellow() { setLightState(secondLight, YELLOW); }
void secondLightRed()    { setLightState(secondLight, RED);    }

void setup()
{
  pinMode(firstLight.greenPin,   OUTPUT);
  pinMode(firstLight.yellowPin,  OUTPUT);
  pinMode(firstLight.redPin,     OUTPUT);
  pinMode(secondLight.greenPin,  OUTPUT);
  pinMode(secondLight.yellowPin, OUTPUT);
  pinMode(secondLight.redPin,    OUTPUT);

  firstLightGreen();
  secondLightRed();
  
  firstLightCycle.at(13,firstLightYellow);
  firstLightCycle.at(15,firstLightRed);
  firstLightCycle.at(30,firstLightGreen);

  secondLightCycle.at(15,secondLightGreen);
  secondLightCycle.at(28,secondLightYellow);
  secondLightCycle.at(30,secondLightRed);
  
  firstLightCycle.start();
  secondLightCycle.start();
}

void loop()
{
  ScheduleTable::update();
}
