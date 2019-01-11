[![GitHub release](https://img.shields.io/github/release/Locoduino/ScheduleTable.svg)](https://github.com/Locoduino/ScheduleTable/releases/latest) [![GitHub commits](https://img.shields.io/github/commits-since/Locoduino/ScheduleTable/1.4.svg)](https://github.com/Locoduino/ScheduleTable/compare/1.4...master) [![Build Status](https://travis-ci.org/Locoduino/ScheduleTable.svg?branch=master)](https://travis-ci.org/Locoduino/ScheduleTable)

# ScheduleTable

A Schedule Table Library for Arduino

## Changelog

- **1.4**     Added ```empty()```, and ```removeAt()``` methods. Objects are usable on ATTiny. No more memory allocation. Added an example with C++ closures.
- **1.3**     Use template for schedule table size. This prevents dynamic memory allocation and allows to use schedule tables on ATTiny.
- **1.2**     Initial release.

## Description

A schedule table is a data structure to schedule actions along a timeline.
A schedule table has a period and can process one or more actions.
For instance one can depict a schedule table with period 500 as follow

<img src="http://locoduino.org/pic/github/st.png">

Actions may be set a dates inside the schedule table.
Suppose you want to flash a LED every 500 ms, a first action, ON may be set
at date 200 and a second one, OFF, may be set at date 250.

<img src="http://locoduino.org/pic/github/sta.png">

When the schedule table is executed action ON is processed at 200 and OFF at
250. When the date reaches 500, the schedule table restarts at 0, so ON is
processed again at date 700 and OFF at date 750 and so on.

To use the ScheduleTable library you have to import the library in the IDE
or insert

```C++
#include <ScheduleTable.h>
```

at the beginning of your sketch.

### Creating a schedule table

To implement our LED behavior, we need a schedule table with 2 actions and a
500ms period. This is done by using a SchedTable class instance :

```C++
SchedTable<2> blinkLED(500);
```

By default the time base is in milliseconds. If a different timebase is needed,
a third argument may be used to set the timebase. For example the following
declaration would set the timebase of the schedule table to 50ms so that
period is now expressed in multiple of 50ms.

```C++
SchedTable<2> blinkLED(10,50);
```

Each schedule table may have their own timebase.

A Schedule Table cannot handle more than 255 actions.

The period cannot be greater than 2<sup>31</sup>-1 divided by the timebase. This allows to up to more than 2 billions of milliseconds, that is 24 days.

### Adding actions

Actions are functions called by the schedule table. An action is a function
that returns nothing and has no argument. Let's define 2 actions for our led,
ledOn and ledOff:

```C++
void ledOn()
{
  digitalWrite(13, HIGH);
}

void ledOff()
{
  digitalWrite(13, LOW);
}
```

These actions are now added to the schedule table. This is done in the setup
function along with a pinMode to set digital I/O 13 in OUTPUT.

```C++
pinMode(13, OUTPUT);
blinkLED.at(200,ledOn);
blinkLED.at(250,ledOff);
```

Actions may be inserted at a date greater than the period by will not be processed until the period is changed so that the date is lower than or equal to the period.

The dates of actions cannot be set at a date greater than 2<sup>31</sup>-1 divided by the timebase. This allows to up to more than 2 billions of milliseconds, that is 24 days to schedule actions.

### Starting a schedule table

A schedule table must be started to process the actions. This is done by
calling the method start. Without any argument, the schedule table is started
for an infinite number of periods. For a fixed number of period, this number
is given as argument.

```C++
blinkLED.start(); // starts blinkLED for an infinite number of period
```

or

```C++
blinkLED.start(10); // starts blinkLED for 10 periods
```

### Stopping a schedule table

A schedule table may be stopped by calling the method stop

```C++
blinkLED.stop(); // stop blinkLED
```

### Setting the period

The period of a schedule table may be changed during the execution of the
application by calling the setPeriod method

```C++
blinkLED.setPeriod(1000); // change blinkLED period to 1000
```

If a shorter period is used, actions with date greater  than the period are not processed
anymore

### Removing actions

Actions can be removed by using the removeAt method on a stopped schedule table.
If called on a running schedule table, this method has no effect.
This method takes the date as argument. If there is no action at the given date,
removeAt has no effect. If there is more than one action at the given date, they
are all removed.

```C++
myScheduleTable.removeAt(300);
```

removes the action(s) standing at date 300.

### Emptying a schedule table

All actions can be removed by using the empty method. The schedule table is stopped.

```C++
myScheduleTable.empty();
```

### Processing a schedule table

Schedule tables should be updated as fast as possible and at least every
millisecond if you want to have the actions processed at a reasonably accurate
date. This is done by calling the class method update in loop.

```C++
ScheduleTable::update()
```

As a result the current date of all the schedule tables is updated and
processing of action is done.

## Full example

Here is the full example of blinkLED

```C++
#include <ScheduleTable.h>

SchedTable<2> blinkLED(500);

void ledOn()
{
  digitalWrite(13, HIGH);
}

void ledOff()
{
  digitalWrite(13, LOW);
}

void setup() {
  pinMode(13, OUTPUT);

  blinkLED.at(200,ledOn);
  blinkLED.at(250,ledOff);

  blinkLED.start();
}

void loop() {
  ScheduleTable::update();
}
```

For more examples checks the examples directory.

## Note about Travis CI

The ScheduleTable Library examples are built on Travis CI for the following boards:

- Arduino Leonardo
- Arduino Uno
- Arduino Mega 2560
- Arduino Zero
- Arduino Due
