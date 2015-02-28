# ScheduleTable

A Schedule Table Library for Arduino

A schedule table is a data structure to schedule actions along a timeline.
A schedule table has a period and can manage one or more actions.
For instance one can depict a schedule table with period 32 as follow

<img src="https://dl.dropboxusercontent.com/u/67740546/st.png">

Actions may be set a dates inside the schedule table.
Suppose you want to flash a LED every 500 ms, a first action, ON may be set
at date 200 and a second one, OFF, may be set at date 250.

<img src="https://dl.dropboxusercontent.com/u/67740546/sta.png">

When the schedule table is executed action ON is triggered at 200 and OFF at
250. When the date reaches 500, the schedule table restarts at 0, so ON is
triggered again at date 700 and OFF at date 750 and so on.

To use the ScheduleTable library you have to import the library in the IDE
or insert

```
#include <ScheduleTable.h>
```

at the beginning of your sketch.

## Creating a schedule table

To implement our LED behavior, we need a schedule table with 2 actions and a
500ms period. This is done by using a ScheduleTable class instance :

```
ScheduleTable blinkLED(2,500);
```

By default the time base is in milliseconds. If a different timebase is needed,
a third argument may be used to set the timebase. For instance the following
declaration would set the timebase of the schedule table to 50ms so that
period is now expressed in multiple of 50ms.

```
ScheduleTable blinkLED(2,10,50);
```

Each schedule table may have their own timebase.

## Adding actions

Actions are functions called by the schedule table. An action is a function
that returns nothing and has no argument. Let's define 2 actions for our led,
ledOn and ledOff:

```
void ledOn()
{
  digitalWrite(13, HIGH);
}

void ledOff()
{
  digitalWrite(13, LOW);
}
```

