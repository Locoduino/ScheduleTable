/*
 * Schedule tables for Arduino
 * 
 * Copyright Jean-Luc Béchennec 2015
 *
 *  This software is distributed under the GNU Public Licence v2 (GPLv2)
 *
 * Please read the LICENCE file
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ScheduleTable.h"

/*
 * actions as object are only possible on high end Arduino, not on ATTiny
 * because the later cannot do dynamic memory allocation
 */
#if !defined(ARDUINO_AVR_ATTINYX5) 

void ScheduleTableActionSlot::print()
{
  Serial.println(mOffset);
}

#endif

ScheduleTable *ScheduleTable::scheduleTableList = NULL;

void ScheduleTable::insertAction(
  unsigned long offset,
#ifndef SCHEDTABLE_RUNNING_LOW_END 
  ScheduleTableAction *action
#else
	function						action
#endif
  )
{
  ScheduleTableActionSlot point(offset, action);
  ScheduleTableActionSlot *slots = storage();
  /* find its place in the table */
  for (byte index = 0; index < mSize; index++) {
    if (point < slots[index]) {
      ScheduleTableActionSlot tmp = slots[index];
      slots[index] = point;
      point = tmp;
    }
  }
  slots[mSize++] = point;
}

void ScheduleTable::at(
  unsigned long offset,
  function action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    if (offset <= mPeriod) {
    
#ifndef SCHEDTABLE_RUNNING_LOW_END 
      insertAction(offset, new FunctionCallAction(action));
#else
      insertAction(offset, action);
#endif
    }
  }
}

#ifndef SCHEDTABLE_RUNNING_LOW_END 
void ScheduleTable::at(
  unsigned long offset,
  ScheduleTableAction& action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    if (offset <= mPeriod) {
      insertAction(offset, &action);
    }
  }
}
#endif

void ScheduleTable::updateIt()
{
  if (mState != SCHEDULETABLE_STOPPED) {
    unsigned long currentDate = millis();
    unsigned long offsetDate = currentDate - mOrigin;
    ScheduleTableActionSlot *slots = storage();
    
    while (mCurrent < mSize &&
           slots[mCurrent].perform(offsetDate, mPeriod)) {
      mCurrent++;
    }
    if (mCurrent == mSize) {
      mCurrent = 0;
      if (mState == SCHEDULETABLE_PERIODIC_FOREVER ||
          (mState == SCHEDULETABLE_PERIODIC && --mHowMuch != 0)) {
        mOrigin += mPeriod;
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

void ScheduleTable::setPeriod(unsigned int period)
{
  mPeriod = period * mTimeBase;
}

void ScheduleTable::print()
{
  Serial.print('['); Serial.print(mSize); Serial.println("]");
  ScheduleTableActionSlot *slots = storage();
  for (byte index = 0; index < mSize; index++) {
    slots[index].print();
  }
}

void ScheduleTable::update()
{
  ScheduleTable *currentScheduleTable = scheduleTableList;
  while (currentScheduleTable) {
    currentScheduleTable->updateIt();
    currentScheduleTable = currentScheduleTable->mNext;
  }
}