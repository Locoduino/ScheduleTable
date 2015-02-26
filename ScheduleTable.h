/*
 * Schedule tables for Arduino
 */
#include "Arduino.h"

typedef void (*function)();

class ScheduleTableAction
{
private:
  unsigned long mOffset;
  function mCallback;
  
public:
  ScheduleTableAction();

  ScheduleTableAction(
  	unsigned long offset,
  	function callback) :
  	mOffset(offset), mCallback(callback) {}
  
  bool perform(unsigned long offsetDate, unsigned long period) {
    if (offsetDate <= period && offsetDate >= mOffset) {
      if (mCallback) mCallback();
      return true;
    }
    return false;      
  }
  
  ScheduleTableAction& operator=(const ScheduleTableAction& action) {
  	mOffset = action.mOffset;
  	mCallback = action.mCallback;
  	return *this;
  }
  
  bool operator<(const ScheduleTableAction& action) {
  	return (mOffset < action.mOffset);
  }

  bool operator<=(const ScheduleTableAction& action) {
  	return (mOffset <= action.mOffset);
  }

  bool operator>(const ScheduleTableAction& action) {
  	return (mOffset > action.mOffset);
  }

  bool operator>=(const ScheduleTableAction& action) {
  	return (mOffset >= action.mOffset);
  }
  
  void print();
};

enum {
  SCHEDULETABLE_STOPPED,
  SCHEDULETABLE_PERIODIC,
  SCHEDULETABLE_PERIODIC_FOREVER
};

class ScheduleTable
{
private:
  unsigned long mTimeBase;
  unsigned long mOrigin;
  unsigned long mPeriod;
  ScheduleTableAction *mAction;
  unsigned int mHowMuch;
  byte mSize;
  byte mMaxSize;
  byte mCurrent;
  byte mState;
  ScheduleTable *mNext;
  
  static ScheduleTable *scheduleTableList;
  
  /* Look for action to perform */
  void updateIt();

public:
  /* Constructor. Does the initialization of the Schedule Table */
  ScheduleTable(byte size, unsigned long period, unsigned long timeBase = 1) :
    mTimeBase(timeBase),
    mPeriod(period * timeBase),
    mSize(0),
    mMaxSize(size),
    mCurrent(0),
    mState(SCHEDULETABLE_STOPPED)
  {
  	mAction = new ScheduleTableAction[size];
  	mNext = scheduleTableList;
  	scheduleTableList = this;
  }

  /* Schedule a new action */
  void at(unsigned long offset, function action);
  /* Start the schedule table periodic or one shot */
  void start(unsigned int howMuch = 0);
  /* Stop the schedule table */
  void stop();
  /* Set the period of the schedule table */
  void setPeriod(unsigned int period);
  /* Print the whole schedule table for debugging purpose */
  void print();
  
  static void update();
};
  
