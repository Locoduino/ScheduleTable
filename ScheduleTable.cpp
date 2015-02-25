/*
 * Schedule tables for Arduino
 */

#include "ScheduleTable.h"

ScheduleTableAction::ScheduleTableAction()
{
	mOffset = 0;
	mCallback = NULL;
}

void ScheduleTableAction::print()
{
  Serial.println(mOffset);
}

void ScheduleTable::at(
  unsigned long offset,
  function action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    if (offset <= mPeriod) {
		ScheduleTableAction point(offset, action);
		/* find its place in the table */
		for (byte index = 0; index < mSize; index++) {
		  if (point < mAction[index]) {
			ScheduleTableAction tmp = mAction[index];
			mAction[index] = point;
			point = tmp;
		  }
		}
		mAction[mSize++] = point;
	 }
  }
}

void ScheduleTable::update()
{
  if (mState != SCHEDULETABLE_STOPPED) {
    unsigned long currentDate = millis();
    unsigned long offsetDate = currentDate - mOrigin;
    while (mCurrent < mSize && mAction[mCurrent].perform(offsetDate)) {
      mCurrent++;
    }
    if (mCurrent == mSize) {
      mCurrent = 0;
      if (mState == SCHEDULETABLE_PERIODIC_FOREVER ||
          (mState == SCHEDULETABLE_PERIODIC && --mHowMuch != 0)) {
        mOrigin = currentDate;
      }
      else {
        mState = SCHEDULETABLE_STOPPED;
      }
    } 
  }
}

/* Start the schedule table periodic */
void ScheduleTable::start(unsigned int howMuch)
{
  mOrigin = millis();
  mCurrent = 0;
  mHowMuch = howMuch;
  if (howMuch) mState = SCHEDULETABLE_PERIODIC;
  else         mState = SCHEDULETABLE_PERIODIC_FOREVER;
}

/* Stop the schedule table */
void ScheduleTable::stop()
{
  mState = SCHEDULETABLE_STOPPED;
}
  
void ScheduleTable::print()
{
  Serial.print('['); Serial.print(mSize); Serial.println("]");
  for (byte index = 0; index < mSize; index++) {
    mAction[index].print();
  }
}
  
