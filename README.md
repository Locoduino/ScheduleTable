# ScheduleTable

A Schedule Table Library for Arduino

A schedule table is a data structure to schedule actions along a timeline.
A schedule table has a period and can manage one or more actions.
For instance one can depict a schedule table with period 500 as follow

|---------|---------|---------|---------|---------|
0        100       200       300       400       500

Actions may be set a dates inside the schedule table.
Suppose you want to flash a LED every 500 ms, a first action, ON may be set
at date 200 and a second one, OFF, may be set at date 250.

                    ON  OFF 
                    |    |
|---------|---------|----|----|---------|---------|
0        100       200       300       400       500

When the schedule table is executed action ON is triggered at 200 and OFF at
250. When the date reaches 500, the schedule table restarts at 0, so ON is
triggered again at date 700 and OFF at date 750 and so on.

# Creating a schedule table

