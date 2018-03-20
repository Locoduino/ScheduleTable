/*
 * Very simple example of the use of the ScheduleTable library
 *
 * This example uses a unique ScheduleTable with one action which prints
 * the number of execution of the action on Serial.
 * The period of the schedule table is 1000ms and the action is
 * set at the end of the period.
 */

#include <ScheduleTable.h>

SchedTable<1> helloCycle(1000);

void hello()
{
  static byte count = 0;

  Serial.print(++count);
  Serial.print(" Hello it is ");
  Serial.println(millis());
}

void setup()
{
  Serial.begin(9600);

  /* the action is set à 1000ms from start of the period */
  helloCycle.at(1000,hello);

  /* start the schedule table for 10 times */
  helloCycle.start(10);
}

void loop()
{
  ScheduleTable::update();
}
