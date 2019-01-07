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

#ifndef __ScheduleTable_h__
#define __ScheduleTable_h__

#include "Arduino.h"

#if defined(ARDUINO_AVR_ATTINYX5) || defined(ARDUINO_AVR_ATTINYX4)
#define SCHEDTABLE_RUNNING_LOW_END
#endif

typedef void (*function)();

/*
 * actions as object are only possible on high end Arduino, not on ATTiny
 * because the later cannot do dynamic memory allocation
 */
#ifndef SCHEDTABLE_RUNNING_LOW_END

class ScheduleTableAction
{
  friend class ScheduleTable;
private:
  bool mHasBeenAllocatedByScheduleTable = false;
public:
  bool hasBeenAllocatedByScheduleTable() {
    return mHasBeenAllocatedByScheduleTable;
  }
  /* action to be performed */
  virtual void action() = 0;

  virtual ~ScheduleTableAction() {
    Serial.print(F("Delete: "));
    Serial.println((uint32_t)this);
  }
};

#endif

class ScheduleTableActionSlot
{
  friend class ScheduleTable;
private:
  uint32_t       mOffset;

#ifndef SCHEDTABLE_RUNNING_LOW_END
  ScheduleTableAction *mAction;
#else
	function            mAction;
#endif

public:
  ScheduleTableActionSlot() : mOffset(0), mAction(NULL) {}

#ifndef SCHEDTABLE_RUNNING_LOW_END
  ScheduleTableActionSlot(uint32_t offset, ScheduleTableAction *action) :
  	mOffset(offset), mAction(action) {}
#else
  ScheduleTableActionSlot(uint32_t offset, function action) :
  	mOffset(offset), mAction(action) {}
#endif

  bool perform(uint32_t offsetDate, uint32_t period) {
    if (offsetDate <= period && offsetDate >= mOffset) {
#ifndef SCHEDTABLE_RUNNING_LOW_END
      mAction->action();
#else
	    mAction();
#endif
      return true;
    }
    return false;
  }

  virtual ScheduleTableActionSlot& operator=(
  	const ScheduleTableActionSlot& actionSlot)
  {
  	mOffset = actionSlot.mOffset;
  	mAction = actionSlot.mAction;
  	return *this;
  }

  bool operator<(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset < actionSlot.mOffset);
  }

  bool operator<=(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset <= actionSlot.mOffset);
  }

  bool operator>(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset > actionSlot.mOffset);
  }

  bool operator>=(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset >= actionSlot.mOffset);
  }

  void print();
};

#ifndef SCHEDTABLE_RUNNING_LOW_END
class FunctionCallAction : public ScheduleTableAction
{
private:
  function mCallback;

public:
  FunctionCallAction(function callback) : mCallback(callback) {}

  virtual void action() { if (mCallback) mCallback(); }
};
#endif

enum {
  SCHEDULETABLE_STOPPED,
  SCHEDULETABLE_PERIODIC,
  SCHEDULETABLE_PERIODIC_FOREVER
};

class ScheduleTable
{
private:
  uint32_t mTimeBase;
  uint32_t mOrigin;
  uint32_t mPeriod;
  unsigned int mHowMuch;
  byte mSize;
  byte mMaxSize;
  byte mCurrent;
  byte mState;
  ScheduleTable *mNext;

  static ScheduleTable *scheduleTableList;

  /* Look for action to perform */
  void updateIt();

  /* insert an action in the table */
#ifndef SCHEDTABLE_RUNNING_LOW_END
  void insertAction(uint32_t offset, ScheduleTableAction *action);
#else
  void insertAction(uint32_t offset, function action);
#endif

  /* get the storage, redefined by inheriting template */
  virtual ScheduleTableActionSlot *storage();

  /* Prevent copy */
  ScheduleTable(const ScheduleTable &inScheduleTable);
  ScheduleTable& operator=(const ScheduleTable &inScheduleTable);

public:
  /* Constructor. Does the initialization of the Schedule Table */
  ScheduleTable(uint8_t size, uint32_t period, uint32_t timeBase) :
    mTimeBase(timeBase),
    mPeriod(period * timeBase),
    mSize(0),
    mMaxSize(size),
    mCurrent(0),
    mState(SCHEDULETABLE_STOPPED)
  {
  	mNext = scheduleTableList;
  	scheduleTableList = this;
  }

  /* Schedule a new action */
  void at(uint32_t offset, function action);

#ifndef SCHEDTABLE_RUNNING_LOW_END
  void at(uint32_t offset, ScheduleTableAction& action);
#endif

  /* Start the schedule table periodic or one shot */
  void start(unsigned int howMuch = 0);
  /* Stop the schedule table */
  void stop();
  /* Set the period of the schedule table */
  void setPeriod(unsigned int period);
  /* Remove all the actions from the schedule table */
  void empty();
  /*
   * Remove an action according to its date. If the Schedule Table is not
   * stopped or if the date does not exist, removeAt has no effect
   */
  void removeAt(const uint32_t inDate);
  /* Print the whole schedule table for debugging purpose */
  void print();

  static void update();
};

template<uint8_t SIZE> class SchedTable : public ScheduleTable
{
public:
  SchedTable(uint32_t period, uint32_t timeBase = 1) :
    ScheduleTable(SIZE, period, timeBase)
  {
  }

private:
  ScheduleTableActionSlot mAction[SIZE];

  ScheduleTableActionSlot *storage() { return mAction; }
};

#endif
