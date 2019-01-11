/*
 * Very simple example of the use of the ScheduleTable library
 *
 * This example uses a unique ScheduleTable with 2 actions which switch
 * the built in led on and off
 * The period of the schedule table is 10 with a timebase of 100ms.
 * The actions are put at 5 (500ms) and 6 (600ms).
 * Actions are added using C++ closures (lamba functions)
 */
#include <ScheduleTable.h>


SchedTable<2> blink(10,100);

void setup() {
  pinMode(13, OUTPUT);
  blink.at(5, []{ digitalWrite(13, HIGH); }); /* use closure ! */
  blink.at(6, []{ digitalWrite(13, LOW); });  /* use closure ! */
  blink.start();
}

void loop() {
  ScheduleTable::update();  
}
