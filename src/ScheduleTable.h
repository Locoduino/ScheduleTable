/*
 * Schedule tables for Arduino
 *
 * Copyright Jean-Luc Béchennec 2015-2019
 *
 * This software is distributed under the GNU Public Licence v2 (GPLv2)
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

#ifndef __SCHEDULETABLE_H__
#define __SCHEDULETABLE_H__

#include "Arduino.h"

/*--------------------------------------------------------------------------------------------------
 * Prototype for functions as actions
 */
typedef void (*function)();

/*--------------------------------------------------------------------------------------------------
 * ScheduleTableAction is an abstract class
 * for actions put in schedule tables
 */
class ScheduleTableAction
{
public:
  /* action to be performed */
  virtual void action() = 0;
};

/*--------------------------------------------------------------------------------------------------
 * ScheduleTableActionSlot is a slot in the schedule table.
 * It encapsulates an action, its date (mOffset) and a boolean
 * which specify whether the action is a simple function call
 * or an object
 */
class ScheduleTableActionSlot
{
private:
  bool      mIsFunction:1;
  uint32_t  mOffset:31;
  void      *mAction;

public:
  /* Empty slot constructor */
  ScheduleTableActionSlot() : mIsFunction(false), mOffset(0), mAction(NULL) {}
  /* Slot constructor */
  ScheduleTableActionSlot(
    const uint32_t  inOffset,
    void            *inAction,
    const bool      inIsFunction
  ) : mIsFunction(inIsFunction), mOffset(inOffset), mAction(inAction) {}
  /*
   * perform the action if the date arrived. Return true if the action
   * has been performed, false otherwise.
   */
  bool perform(uint32_t offsetDate, uint32_t period) {
    if (offsetDate <= period && offsetDate >= mOffset) {
      if (mIsFunction) {
        ((function)mAction)();
      }
      else {
        ((ScheduleTableAction *)mAction)->action();
      }
      return true;
    }
    return false;
  }
  /* copy operator */
  virtual ScheduleTableActionSlot& operator=(
  	const ScheduleTableActionSlot& actionSlot)
  {
    mIsFunction = actionSlot.mIsFunction;
    mOffset = actionSlot.mOffset;
  	mAction = actionSlot.mAction;
  	return *this;
  }
  /* comparison < operator, the comparison is made on the dates  */
  bool operator<(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset < actionSlot.mOffset);
  }
  /* comparison <= operator, the comparison is made on the dates  */
  bool operator<=(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset <= actionSlot.mOffset);
  }
  /* comparison > operator, the comparison is made on the dates  */
  bool operator>(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset > actionSlot.mOffset);
  }
  /* comparison >= operator, the comparison is made on the dates  */
  bool operator>=(const ScheduleTableActionSlot& actionSlot) {
  	return (mOffset >= actionSlot.mOffset);
  }
  /* return the offset */
  uint32_t offset() { return mOffset; }
  /* print the slot for debug purpose */
  void print();
};

/*--------------------------------------------------------------------------------------------------
 * States of a schedule table
 */
enum {
  SCHEDULETABLE_STOPPED,          /* stopped schedule table, initial state */
  SCHEDULETABLE_PERIODIC,         /* running schedule table for a number of period */
  SCHEDULETABLE_PERIODIC_FOREVER  /* running schedule table for for an infinite number of periods */
};

/*--------------------------------------------------------------------------------------------------
 * ScheduleTable class
 */
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
  void insert(const uint32_t inOffset, void *inAction, const bool inIsFunction);
  void insertAction(const uint32_t offset, ScheduleTableAction *action);
  void insertAction(const uint32_t offset, function action);

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
  void at(uint32_t offset, ScheduleTableAction& action);

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
  /* Update all schedule tables */
  static void update();
};

/*--------------------------------------------------------------------------------------------------
 * ScheduleTable template
 */
template<size_t SIZE> class SchedTable : public ScheduleTable
{
  /* Check the size is lower than 256, emit an error otherwise */
  static_assert(SIZE < 256, "Schedule table with size greater than 255 are forbidden");

  /* Check the size is not 0, emit an error otherwise */
  static_assert(SIZE != 0, "Schedule table with zero size are forbidden");

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
