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
  Serial.print(mOffset);
  Serial.print(':');
#ifndef SCHEDTABLE_RUNNING_LOW_END
  if (mAction->hasBeenAllocatedByScheduleTable()) {
    Serial.print(F("+("));
  }
#endif
  Serial.print((uint32_t)mAction);
#ifndef SCHEDTABLE_RUNNING_LOW_END
  if (mAction->hasBeenAllocatedByScheduleTable()) {
    Serial.print(')');
  }
#endif
}

#endif

ScheduleTable *ScheduleTable::scheduleTableList = NULL;

void ScheduleTable::insertAction(
  uint32_t offset,
#ifndef SCHEDTABLE_RUNNING_LOW_END
  ScheduleTableAction *action
#else
  function            action
#endif
  )
{
  ScheduleTableActionSlot point(offset, action);
  ScheduleTableActionSlot *slots = storage();
  /* find its place in the table */
  for (uint8_t index = 0; index < mSize; index++) {
    if (point < slots[index]) {
      ScheduleTableActionSlot tmp = slots[index];
      slots[index] = point;
      point = tmp;
    }
  }
  slots[mSize++] = point;
}

void ScheduleTable::at(
  uint32_t offset,
  function action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    if (offset <= mPeriod) {

#ifndef SCHEDTABLE_RUNNING_LOW_END
      FunctionCallAction *wrappedAction = new FunctionCallAction(action);
      wrappedAction->mHasBeenAllocatedByScheduleTable = true;
      insertAction(offset, wrappedAction);
#else
      insertAction(offset, action);
#endif
    }
  }
}

#ifndef SCHEDTABLE_RUNNING_LOW_END
void ScheduleTable::at(
  uint32_t offset,
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
    uint32_t currentDate = millis();
    uint32_t offsetDate = currentDate - mOrigin;
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

/*
 * Remove all the actions from the schedule table
 */
void ScheduleTable::empty()
{
  ScheduleTableActionSlot *slots = storage();
  for (uint8_t index = 0; index < mSize; index++) {
    if (slots[index].mAction->hasBeenAllocatedByScheduleTable()) {
      delete slots[index].mAction;
    }
  }
  mSize = 0;
}

/*
 * Remove an action according to its date. If the Schedule Table is not
 * stopped or if the date does not exist, removeAt has no effect
 */
void ScheduleTable::removeAt(const uint32_t inDate)
{
  if (mState == SCHEDULETABLE_STOPPED) {
    ScheduleTableActionSlot *slots = storage();
    for (uint8_t index = 0; index < mSize; index++) {
      if (slots[index].mOffset == inDate) {
        if (slots[index].mAction->hasBeenAllocatedByScheduleTable()) {
          delete slots[index].mAction;
        }
        for (uint8_t s = index; s < (mSize - 1); s++) {
          slots[s] = slots[s + 1];
        }
        index--;
        mSize--;
      }
    }
  }
}

void ScheduleTable::print()
{
  Serial.print('['); Serial.print(mSize); Serial.print("] ");
  ScheduleTableActionSlot *slots = storage();
  for (uint8_t index = 0; index < mSize; index++) {
    slots[index].print();
    Serial.print(' ');
  }
  Serial.println();
}

void ScheduleTable::update()
{
  ScheduleTable *currentScheduleTable = scheduleTableList;
  while (currentScheduleTable) {
    currentScheduleTable->updateIt();
    currentScheduleTable = currentScheduleTable->mNext;
  }
}
