/*
 * Schedule tables for Arduino
 *
 * Copyright Jean-Luc Béchennec 2015-2019
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
 * For debug, print a slot
 */
void ScheduleTableActionSlot::print()
{
  Serial.print(mOffset);
  Serial.print(':');
  if (mIsFunction) {
    Serial.print(F("f("));
  }
  Serial.print((uint32_t)mAction);
  if (mIsFunction) {
    Serial.print(')');
  }
}

/*
 * At start the list of schedule tables is empty
 */
ScheduleTable *ScheduleTable::scheduleTableList = NULL;

/*
 * Insert an action in a schedule table.
 * The action is put at its location according to its date
 */
void ScheduleTable::insert(
  const uint32_t  inOffset,
  void            *inAction,
  const bool      inIsFunction)
{
  ScheduleTableActionSlot point(inOffset, inAction, inIsFunction);
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

/*
 * Insert and action which is an object
 */
void ScheduleTable::insertAction(
  const uint32_t      inOffset,
  ScheduleTableAction *inAction)
{
  insert(inOffset, inAction, false);
}

/*
 * Insert an action which is a function
 */
void ScheduleTable::insertAction(
  const uint32_t  inOffset,
  function        inAction)
{
  insert(inOffset, (void *) inAction, true);
}

/*
 * Add an action (function) to the schedule after computing its actual offset
 */
void ScheduleTable::at(
  uint32_t offset,
  function action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    insertAction(offset, action);
  }
}

/*
 * Add an action (object) to the schedule after computing its actual offset
 */
void ScheduleTable::at(
  uint32_t offset,
  ScheduleTableAction& action)
{
  if (mSize < mMaxSize) {
    /* compute the actual offset */
    offset *= mTimeBase;
    insertAction(offset, &action);
  }
}

/*
 * check the schedule table to look for actions to perform
 */
void ScheduleTable::updateIt()
{
  if (mState != SCHEDULETABLE_STOPPED) {
    uint32_t currentDate = millis();
    uint32_t offsetDate = currentDate - mOrigin;
    ScheduleTableActionSlot *slots = storage();
    bool continueProcessing;

    do {
      continueProcessing = slots[mCurrent].perform(offsetDate, mPeriod);
      if (continueProcessing) {
        mCurrent++;
        if (mCurrent == mSize) {
          mCurrent = 0;
          if (mState == SCHEDULETABLE_PERIODIC_FOREVER ||
              (mState == SCHEDULETABLE_PERIODIC && --mHowMuch != 0)) {
            mOrigin += mPeriod;
            offsetDate = currentDate - mOrigin;
          }
          else {
            mState = SCHEDULETABLE_STOPPED;
            continueProcessing = false;
          }
        }
      }
    } while (continueProcessing);
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

/*
 * Change the period of the schedule table
 * Fail silently if period is lower than 1 or greater than
 * 2^31 - 1
 */
void ScheduleTable::setPeriod(unsigned int period)
{
  if (period > 0 && period <= (0x7FFFFFFF / mTimeBase)) {
    mPeriod = period * mTimeBase;
  }
}

/*
 * Remove all the actions from the schedule table
 * Stop it.
 */
void ScheduleTable::empty()
{
  mState = SCHEDULETABLE_STOPPED;
  mSize = 0;
}

/*
 * Remove an action according to its date. If the Schedule Table is not
 * stopped or if the date does not exist, removeAt has no effect
 */
void ScheduleTable::removeAt(const uint32_t inDate)
{
  if (mState == SCHEDULETABLE_STOPPED) {
    const uint32_t actualDate = inDate * mTimeBase;
    ScheduleTableActionSlot *slots = storage();
    for (uint8_t index = 0; index < mSize; index++) {
      if (slots[index].offset() == actualDate) {
        for (uint8_t s = index; s < (mSize - 1); s++) {
          slots[s] = slots[s + 1];
        }
        index--;
        mSize--;
      }
    }
  }
}

/*
 * Print a schedule table for debug purpose
 */
void ScheduleTable::print()
{
  Serial.print('[');
  Serial.print(mSize);
  Serial.print(']');

  Serial.print('(');
  Serial.print(mPeriod);
  Serial.print('/');
  Serial.print(mTimeBase);
  Serial.print(F(") "));

  ScheduleTableActionSlot *slots = storage();
  for (uint8_t index = 0; index < mSize; index++) {
    slots[index].print();
    Serial.print(' ');
  }
  Serial.println();
}

/*
 * update all the schedule tables by iterating over the list
 */
void ScheduleTable::update()
{
  ScheduleTable *currentScheduleTable = scheduleTableList;
  while (currentScheduleTable) {
    currentScheduleTable->updateIt();
    currentScheduleTable = currentScheduleTable->mNext;
  }
}
