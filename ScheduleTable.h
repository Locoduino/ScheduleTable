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

typedef void (*function)();

class ScheduleTableAction
{
public:
  /* action to be performed */
  virtual void action() = 0;
};

class ScheduleTableActionSlot
{
private:
  unsigned long       mOffset;
  ScheduleTableAction *mAction;

public:
  ScheduleTableActionSlot() : mOffset(0), mAction(NULL) {}

  ScheduleTableActionSlot(unsigned long offset, ScheduleTableAction *action) :
  	mOffset(offset), mAction(action) {}
  
  bool perform(unsigned long offsetDate, unsigned long period) {
    if (offsetDate <= period && offsetDate >= mOffset) {
      mAction->action();
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

class FunctionCallAction : public ScheduleTableAction
{
private:
  function mCallback;

public:
  FunctionCallAction(function callback) : mCallback(callback) {}
  	
  virtual void action() { if (mCallback) mCallback(); }
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
  ScheduleTableActionSlot *mAction;
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
  void insertAction(unsigned long offset, ScheduleTableAction *action);

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
  	mAction = new ScheduleTableActionSlot[size];
  	mNext = scheduleTableList;
  	scheduleTableList = this;
  }

  /* Schedule a new action */
  void at(unsigned long offset, function action);
  void at(unsigned long offset, ScheduleTableAction& action);
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
  
#endif
