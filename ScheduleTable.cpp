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

ScheduleTableAction::ScheduleTableAction()
{
	mOffset = 0;
	mCallback = NULL;
}

void ScheduleTableAction::print()
{
  Serial.println(mOffset);
}

ScheduleTable *ScheduleTable::scheduleTableList = NULL;

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

void ScheduleTable::updateIt()
{
  if (mState != SCHEDULETABLE_STOPPED) {
    unsigned long currentDate = millis();
    unsigned long offsetDate = currentDate - mOrigin;
    while (mCurrent < mSize &&
           mAction[mCurrent].perform(offsetDate, mPeriod)) {
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
  for (byte index = 0; index < mSize; index++) {
    mAction[index].print();
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