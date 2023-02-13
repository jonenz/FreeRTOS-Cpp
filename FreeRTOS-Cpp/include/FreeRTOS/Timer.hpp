/*
 * FreeRTOS-Cpp
 * Copyright (C) 2021 Jon Enz. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://github.com/jonenz/FreeRTOS-Cpp
 */

#ifndef FREERTOS_TIMER_HPP
#define FREERTOS_TIMER_HPP

#include "FreeRTOS.h"
#include "timers.h"

/**
 * @brief C function that is used to interface this class with the FreeRTOS
 * kernel.
 *
 * @note This function should not be called or referenced by the user.
 *
 * @param task Pointer to an instance of FreeRTOS::TimerBase.
 */
void callTimerFunction(TimerHandle_t timer);

namespace FreeRTOS {

/**
 * @class TimerBase Timer.hpp <FreeRTOS/Timer.hpp>
 *
 * @brief Base class that provides the standard task interface to
 * FreeRTOS::Timer and FreeRTOS::StaticTimer.
 *
 * @note This class is not intended to be instantiated or derived from by the
 * user. Use FreeRTOS::Timer or FreeRTOS::StaticTimer as a base class for a user
 * implemented task.
 */
class TimerBase {
 public:
  friend class Timer;
  friend class StaticTimer;

  TimerBase(const TimerBase&) = delete;
  TimerBase& operator=(const TimerBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * Timer.hpp
   *
   * @brief Function that acts as the entry point of the timer instance.
   *
   * @note This function is only public so that it can be accessed by the C
   * interface function <tt>callTimerFunction()</tt> and should not be called or
   * referenced by the user.
   */
  virtual inline void timerEntry() final { timerFunction(); }

  /**
   * Timer.hpp
   *
   * @brief Function that checks the value of the timer handle.  This function
   * should be called to ensure the timer was created successfully.
   *
   * @return true If the timer was created successfully.
   * @return false If the timer was not created successfully due to insufficient
   * memory.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerIsTimerActive(
   * TimerHandle_t xTimer )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerIsTimerActive.html>
   *
   * Queries a software timer to see if it is active or dormant.
   *
   * A timer will be dormant if:
   * -# It has been created but not started, or
   * -# It is an expired one-shot timer that has not been restarted.
   *
   * @note Timers are created in the dormant state. The start(), reset(),
   * startFromISR(), resetFromISR(), changePeriod() and changePeriodFromISR()
   * API functions can all be used to transition a timer into the active state.
   *
   * @return false If the timer is dormant.
   * @return true Otherwise.
   *
   * <b>Example Usage</b>
   * @include Timer/isActive.cpp
   */
  inline bool isActive() const {
    return (xTimerIsTimerActive(handle) != pdFALSE);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStart( TimerHandle_t
   * xTimer, TickType_t xBlockTime )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStart.html>
   *
   * start() starts a timer. If the timer had already been started and was
   * already in the active state, then start() has equivalent functionality to
   * the reset() API function.
   *
   * Starting a timer ensures the timer is in the active state. If the timer is
   * not stopped, deleted, or reset in the mean time, timerFunction() will get
   * called 'n 'ticks after start() was called, where 'n' is the timers defined
   * period.
   *
   * It is valid to call start() before the RTOS scheduler has been started, but
   * when this is done the timer will not actually start until the RTOS
   * scheduler is started, and the timers expiry time will be relative to when
   * the RTOS scheduler is started, not relative to when start() was called.
   *
   * @param blockTime Specifies the time, in ticks, that the calling task should
   * be held in the Blocked state to wait for the start command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when start() was called. blockTime is ignored if start() is called
   * before the RTOS scheduler is started.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system,
   * although the timers expiry time is relative to when start() is actually
   * called. The timer service/daemon task priority is set by the
   * configTIMER_TASK_PRIORITY configuration constant.
   * @return false If the start command could not be sent to the timer command
   * queue even after blockTime ticks had passed.
   *
   * <b>Example Usage</b>
   * @include Timer/timer.cpp
   */
  inline bool start(const TickType_t blockTime = 0) const {
    return (xTimerStart(handle, blockTime) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStartFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStartFromISR.html>
   *
   * A version of start() that can be called from an interrupt service routine.
   *
   * @param higherPriorityTaskWoken The timer service/daemon task spends most of
   * its time in the Blocked state, waiting for messages to arrive on the timer
   * command queue. Calling startFromISR() writes a message to the timer command
   * queue, so has the potential to transition the timer service/daemon task out
   * of the Blocked state. If calling startFromISR() causes the timer
   * service/daemon task to leave the Blocked state, and the timer service/
   * daemon task has a priority equal to or greater than the currently executing
   * task (the task that was interrupted), then higherPriorityTaskWoken will get
   * set to true internally within the startFromISR() function. If
   * startFromISR() sets this value to true, then a context switch should be
   * performed before the interrupt exits.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system,
   * although the timers expiry time is relative to when startFromISR() is
   * actually called. The timer service/daemon task priority is set by the
   * configTIMER_TASK_PRIORITY configuration constant.
   * @return false If the start command could not be sent to the timer command
   * queue.
   *
   * <b>Example Usage</b>
   * @include Timer/startFromISR.cpp
   */
  inline bool startFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xTimerStartFromISR(handle, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStartFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStartFromISR.html>
   *
   * @overload
   */
  inline bool startFromISR() const {
    return (xTimerStartFromISR(handle, NULL) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStop( TimerHandle_t xTimer,
   * TickType_t xBlockTime )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStop.html>
   *
   * stop() stops a timer that was previously started using either of the
   * start(), reset(), startFromISR(), resetFromISR(), changePeriod() and
   * changePeriodFromISR() API functions.
   *
   * Stopping a timer ensures the timer is not in the active state.
   *
   * @param blockTime Specifies the time, in ticks, that the calling task should
   * be held in the Blocked state to wait for the stop command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when stop() was called. blockTime is ignored if stop() is called
   * before the RTOS scheduler is started.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system. The
   * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
   * configuration constant.
   * @return false If the stop command could not be sent to the timer command
   * queue even after blockTime ticks had passed.
   *
   * <b>Example Usage</b>
   * @include Timer/timer.cpp
   */
  inline bool stop(const TickType_t blockTime = 0) const {
    return (xTimerStop(handle, blockTime) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStopFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStopFromISR.html>
   *
   * A version of stop() that can be called from an interrupt service routine.
   *
   * @param higherPriorityTaskWoken The timer service/daemon task spends most of
   * its time in the Blocked state, waiting for messages to arrive on the timer
   * command queue. Calling stopFromISR() writes a message to the timer command
   * queue, so has the potential to transition the timer service/daemon task out
   * of the Blocked state. If calling stopFromISR() causes the timer
   * service/daemon task to leave the Blocked state, and the timer service/
   * daemon task has a priority equal to or greater than the currently executing
   * task (the task that was interrupted), then higherPriorityTaskWoken will get
   * set to true internally within the stopFromISR() function. If stopFromISR()
   * sets this value to true, then a context switch should be performed before
   * the interrupt exits.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system. The
   * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
   * configuration constant.
   * @return false If the start command could not be sent to the timer command
   * queue.
   *
   * <b>Example Usage</b>
   * @include Timer/stopFromISR.cpp
   */
  inline bool stopFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xTimerStopFromISR(handle, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerStopFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerStopFromISR.html>
   *
   * @overload
   */
  inline bool stopFromISR() const {
    return (xTimerStopFromISR(handle, NULL) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerChangePeriod( TimerHandle_t
   * xTimer, TickType_t xNewPeriod, TickType_t xBlockTime )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerChangePeriod.html>
   *
   * changePeriod() changes the period of a timer.
   *
   * changePeriod() can be called to change the period of an active or dormant
   * state timer. Changing the period of a dormant timers will also start the
   * timer.
   *
   * @param newPeriod The new period for timer.  Timer periods are specified in
   * tick periods, so the constant portTICK_PERIOD_MS can be used to convert a
   * time that has been specified in milliseconds.  For example, if the timer
   * must expire after 100 ticks, then newPeriod should be set to 100.
   * Alternatively, if the timer must expire after 500ms, then newPeriod can be
   * set to ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less
   * than or equal to 1000.
   * @param blockTime Specifies the time, in ticks, that the calling task should
   * be held in the Blocked state to wait for the change period command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when changePeriod() was called. blockTime is ignored if changePeriod()
   * is called before the RTOS scheduler is started.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system. The
   * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
   * configuration constant.
   * @return false If the change period command could not be sent to the timer
   * command queue even after blockTime ticks had passed.
   *
   * <b>Example Usage</b>
   * @include Timer/changePeriod.cpp
   */
  inline bool changePeriod(const TickType_t newPeriod,
                           const TickType_t blockTime = 0) const {
    return (xTimerChangePeriod(handle, newPeriod, blockTime) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerChangePeriodFromISR(
   * TimerHandle_t xTimer, TickType_t xNewPeriod, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see
   * <https://www.freertos.org/FreeRTOS-timers-xTimerChangePeriodFromISR.html>
   *
   * A version of changePeriod() that can be called from an interrupt service
   * routine.
   *
   * @param newPeriod The new period for timer.  Timer periods are specified in
   * tick periods, so the constant portTICK_PERIOD_MS can be used to convert a
   * time that has been specified in milliseconds.  For example, if the timer
   * must expire after 100 ticks, then newPeriod should be set to 100.
   * Alternatively, if the timer must expire after 500ms, then newPeriod can be
   * set to ( 500 / portTICK_PERIOD_MS ) provided configTICK_RATE_HZ is less
   * than or equal to 1000.
   * @param higherPriorityTaskWoken The timer service/daemon task spends most of
   * its time in the Blocked state, waiting for messages to arrive on the timer
   * command queue. Calling changePeriodFromISR() writes a message to the timer
   * command queue, so has the potential to transition the timer service/ daemon
   * task out of the Blocked state. If calling changePeriodFromISR() causes the
   * timer service/daemon task to leave the Blocked state, and the timer
   * service/daemon task has a priority equal to or greater than the currently
   * executing task (the task that was interrupted), then
   * higherPriorityTaskWoken will get set to true internally within the
   * changePeriodFromISR() function. If changePeriodFromISR() sets this value to
   * true, then a context switch should be performed before the interrupt exits.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system. The
   * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
   * configuration constant.
   * @return false If the change period command could not be sent to the timer
   * command queue.
   *
   * <b>Example Usage</b>
   * @include Timer/changePeriodFromISR.cpp
   */
  inline bool changePeriodFromISR(bool& higherPriorityTaskWoken,
                                  const TickType_t newPeriod) const {
    BaseType_t taskWoken = pdFALSE;
    bool result =
        (xTimerChangePeriodFromISR(handle, newPeriod, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerChangePeriodFromISR(
   * TimerHandle_t xTimer, TickType_t xNewPeriod, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see
   * <https://www.freertos.org/FreeRTOS-timers-xTimerChangePeriodFromISR.html>
   *
   * @overload
   */
  inline bool changePeriodFromISR(const TickType_t newPeriod) const {
    return (xTimerChangePeriodFromISR(handle, newPeriod, NULL) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerDelete( TimerHandle_t
   * xTimer, TickType_t xBlockTime )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerDelete.html>
   *
   * deleteTimer() deletes a timer from the FreeRTOS timer task.
   *
   * @note This function is also called in the destructor of the timer using the
   * deleteBlockTime specified when the object was created.  This function
   * should be used when the user wants to delete the timer from the FreeRTOS
   * timer task without destroying the timer object or with a different block
   * time than what was specified in the constructor.
   *
   * @param blockTime Specifies the time, in ticks, that the calling task should
   * be held in the Blocked state to wait for the delete command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when deleteTimer() was called. blockTime is ignored if deleteTimer()
   * is called before the RTOS scheduler is started.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system. The
   * timer service/daemon task priority is set by the configTIMER_TASK_PRIORITY
   * configuration constant.
   * @return false If the delete command could not be sent to the timer command
   * queue even after blockTime ticks had passed.
   *
   * <b>Example Usage</b>
   * @include Timer/changePeriod.cpp
   */
  inline bool deleteTimer(const TickType_t blockTime = 0) {
    if (xTimerDelete(handle, blockTime) == pdPASS) {
      handle = NULL;
      return true;
    }
    return false;
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt> BaseType_t xTimerReset( TimerHandle_t
   * xTimer, TickType_t xBlockTime )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerReset.html>
   *
   * reset() re-starts a timer.  If the timer had already been started and was
   * already in the active state, then reset() will cause the timer to
   * re-evaluate its expiry time so that it is relative to when reset() was
   * called.  If the timer was in the dormant state then reset() has equivalent
   * functionality to the start() API function.
   *
   * Resetting a timer ensures the timer is in the active state.  If the timer
   * is not stopped, deleted, or reset in the mean time, the callback function
   * associated with the timer will get called 'n' ticks after reset() was
   * called, where 'n' is the timers defined period.
   *
   * It is valid to call reset() before the RTOS scheduler has been started, but
   * when this is done the timer will not actually start until the RTOS
   * scheduler is started, and the timers expiry time will be relative to when
   * the RTOS scheduler is started, not relative to when reset() was called.
   *
   * @param blockTime Specifies the time, in ticks, that the calling task should
   * be held in the Blocked state to wait for the reset command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when reset() was called. blockTime is ignored if reset() is called
   * before the RTOS scheduler is started.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system,
   * although the timers expiry time is relative to when reset() is actually
   * called.  The timer service/daemon task priority is set by the
   * configTIMER_TASK_PRIORITY configuration constant.
   * @return false If the reset command could not be sent to the timer command
   * queue even after blockTime ticks had passed.
   *
   * <b>Example Usage</b>
   * @include Timer/reset.cpp
   */
  inline bool reset(const TickType_t blockTime = 0) const {
    return (xTimerReset(handle, blockTime) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerResetFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerResetFromISR.html>
   *
   * A version of reset() that can be called from an interrupt service routine.
   *
   * @param higherPriorityTaskWoken The timer service/daemon task spends most of
   * its time in the Blocked state, waiting for messages to arrive on the timer
   * command queue. Calling resetFromISR() writes a message to the timer command
   * queue, so has the potential to transition the timer service/daemon task out
   * of the Blocked state.  If calling resetFromISR() causes the timer
   * service/daemon task to leave the Blocked state, and the timer
   * service/daemon task has a priority equal to or greater than the currently
   * executing task (the task that was interrupted), then
   * higherPriorityTaskWoken will get set to true internally within the
   * resetFromISR() function. If resetFromISR() sets this value to true, then a
   * context switch should be performed before the interrupt exits.
   * @return true If the command was successfully sent to the timer command
   * queue. When the command is actually processed will depend on the priority
   * of the timer service/daemon task relative to other tasks in the system,
   * although the timers expiry time is relative to when resetFromISR() is
   * actually called. The timer service/daemon task priority is set by the
   * configTIMER_TASK_PRIORITY configuration constant.
   * @return false If the change period command could not be sent to the timer
   * command queue.
   *
   * <b>Example Usage</b>
   * @include Timer/resetFromISR.cpp
   */
  inline bool resetFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xTimerResetFromISR(handle, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTimerResetFromISR( TimerHandle_t
   * xTimer, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerResetFromISR.html>
   *
   * @overload
   */
  inline bool resetFromISR() const {
    return (xTimerResetFromISR(handle, NULL) == pdPASS);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>void vTimerSetReloadMode( TimerHandle_t
   * xTimer, const UBaseType_t uxAutoReload )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-Timers-vTimerSetReloadMode.html>
   *
   * Updates the 'mode' of a software timer to be either an auto reload timer or
   * a one-shot timer.
   *
   * An auto reload timer resets itself each time it expires, causing the timer
   * to expire (and therefore execute its callback) periodically.
   *
   * A one shot timer does not automatically reset itself, so will only expire
   * (and therefore execute its callback) once unless it is manually restarted.
   *
   * @param autoReload Set autoReload to true to set the timer into auto reload
   * mode, or false to set the timer into one shot mode.
   */
  inline void setReloadMode(const bool autoReload) const {
    vTimerSetReloadMode(handle, (autoReload ? pdTRUE : pdFALSE));
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>const char * pcTimerGetName( TimerHandle_t
   * xTimer )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-pcTimerGetName.html>
   *
   * Returns the human readable text name of a software timer.
   *
   * Text names are assigned to timers in the constructor.
   *
   * @return const char* A pointer to the text name of the timer as a standard
   * NULL terminated C string.
   *
   * <b>Example Usage</b>
   * @include Timer/getName.cpp
   */
  inline const char* getName() const { return pcTimerGetName(handle); }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>TickType_t xTimerGetPeriod( TimerHandle_t
   * xTimer )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerGetPeriod.html>
   *
   * Returns the period of a software timer. The period is specified in ticks.
   *
   * The period of a timer is initially set using the period parameter of the
   * constructor. It can then be changed using the changePeriod() and
   * changePeriodFromISR() API functions.
   *
   * @return TickType_t The period of the timer, in ticks.
   *
   * <b>Example Usage</b>
   * @include Timer/getPeriod.cpp
   */
  inline TickType_t getPeriod() const { return xTimerGetPeriod(handle); }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>TickType_t xTimerGetExpiryTime(
   * TimerHandle_t xTimer )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerGetExpiryTime.html>
   *
   * Returns the time at which the software timer will expire, which is the time
   * at which the timer's callback function will execute.
   *
   * If the value returned by getExpiryTime() is less than the current time then
   * the timer will expire after the tick count has overflowed and wrapped back
   * to 0. Overflows are handled in the RTOS implementation itself, so a timer's
   * callback function will execute at the correct time whether it is before or
   * after the tick count overflows.
   *
   * @return TickType_t If the timer is active, then the time at which the timer
   * will next expire is returned (which may be after the current tick count has
   * overflowed, see the notes above).  If the timer is not active then the
   * return value is undefined.
   *
   * <b>Example Usage</b>
   * @include Timer/getExpiryTime.cpp
   */
  inline TickType_t getExpiryTime() const {
    return xTimerGetExpiryTime(handle);
  }

  /**
   * Timer.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxTimerGetReloadMode(
   * TimerHandle_t xTimer )</tt>
   *
   * @see <https://www.freertos.org/uxTimerGetReloadMode.html>
   *
   * Queries the 'mode' of the software timer.
   *
   * The mode can be either an auto-reloaded timer, which automatically resets
   * itself each time it expires, or a one-shot timer, which will expire only
   * once unless it is manually restarted.
   *
   * @return true If the timer is an auto-reload timer.
   * @return false Otherwise.
   */
  inline bool getReloadMode() const {
    return (uxTimerGetReloadMode(handle) == pdTRUE);
  }

  /**
   * Timer.hpp
   *
   * @brief Set the delete block time.  This value is used when the destructor
   * calls deleteTimer().
   *
   * @param deleteBlockTime Delete block time to be set in ticks.
   */
  inline void setDeleteBlockTime(const TickType_t deleteBlockTime = 0) {
    this->deleteBlockTime = deleteBlockTime;
  }

  /**
   * Timer.hpp
   *
   * @brief Set the delete block time.  This value is used when the destructor
   * calls deleteTimer().
   *
   * @return TickType_t Delete block time in ticks.
   */
  inline TickType_t getDeleteBlockTime() const { return deleteBlockTime; }

 protected:
  /**
   * Timer.hpp
   *
   * @brief Abstraction function that acts as the entry point of the timer
   * callback for the user.
   */
  virtual void timerFunction() = 0;

 private:
  /**
   * Timer.hpp
   *
   * @brief Construct a new TimerBase object.  This default constructor is
   * deliberately private as this class is not intended to be instantiated or
   * derived from by the user.  Use FreeRTOS::Timer or FreeRTOS::StaticTimer as
   * a base class for creating a task.
   *
   * @param deleteBlockTime Set the delete block time.  This value is used when
   * the destructor calls deleteTimer().
   */
  explicit TimerBase(const TickType_t deleteBlockTime = 0)
      : deleteBlockTime(deleteBlockTime) {}

  /**
   * Timer.hpp
   *
   * @brief Destroy the TimerBase object.
   *
   * @note This destructor will check that the timer is still valid and has not
   * already been deleted by deleteTimer() before calling the function.  If the
   * timer is still valid the destructor will call deleteTimer() and block for
   * up to the amount of time specified by deleteBlockTime.
   */
  ~TimerBase() {
    if (isValid()) {
      deleteTimer(getDeleteBlockTime());
    }
  }

  TimerBase(TimerBase&&) noexcept = default;
  TimerBase& operator=(TimerBase&&) noexcept = default;

  TimerHandle_t handle = NULL;
  TickType_t deleteBlockTime;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * Timer Timer.hpp <FreeRTOS/Timer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS timer.
 *
 * Each software timer requires a small amount of RAM that is used to hold the
 * timer's state. If a timer is created using this class then this RAM is
 * automatically allocated from the FreeRTOS heap. If a software timer is
 * created using FreeRTOS::StaticTimer() then the RAM is included in the object
 * so it can be statically allocated at compile time. See the Static Vs Dynamic
 * allocation page for more information.
 *
 * @note This class is not intended to be instantiated by the user.  The user
 * should create a class that derives from this class and implement
 * timerFunction().
 */
class Timer : public TimerBase {
 public:
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

 protected:
  /**
   * Timer.hpp
   *
   * @brief Construct a new Timer object by calling <tt>TimerHandle_t
   * xTimerCreate( const char * const pcTimerName, const TickType_t
   * xTimerPeriod, const UBaseType_t uxAutoReload, void * const pvTimerID,
   * TimerCallbackFunction_t pxCallbackFunction )</tt>
   *
   * @see <https://www.freertos.org/FreeRTOS-timers-xTimerCreate.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * timer was created successfully in case the memory required to create the
   * timer could not be allocated.
   *
   * @note Timers are created in the dormant state. The start(), reset(),
   * startFromISR(), resetFromISR(), changePeriod() and changePeriodFromISR()
   * API functions can all be used to transition a timer into the active state.
   *
   * @note When calling <tt>xTimerCreate</tt> the constructor passes the
   * <tt>this</tt> pointer as the pvTimerID argument. This pointer is used so
   * that the interface function callTimerFunction() can invoke timerFunction()
   * for this instance of the class.
   *
   * @param period The period of the timer. The period is specified in ticks,
   * and the macro pdMS_TO_TICKS() can be used to convert a time specified in
   * milliseconds to a time specified in ticks. For example, if the timer must
   * expire after 100 ticks, then simply set period to 100. Alternatively, if
   * the timer must expire after 500ms, then set period to pdMS_TO_TICKS( 500 ).
   * pdMS_TO_TICKS() can only be used if configTICK_RATE_HZ is less than or
   * equal to 1000. The timer period must be greater than 0.
   * @param autoReload If autoReload is set to true, then the timer will expire
   * repeatedly with a frequency set by the period parameter. If autoReload is
   * set to false, then the timer will be a one-shot and enter the dormant state
   * after it expires.
   * @param name A human readable text name that is assigned to the timer.  This
   * is done purely to assist debugging. The RTOS kernel itself only ever
   * references a timer by its handle, and never by its name.
   * @param deleteBlockTime Specifies the time, in ticks, that the calling task
   * should be held in the Blocked state to wait for the delete command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when the destructor is called. deleteBlockTime is ignored if the
   * destructor is called before the RTOS scheduler is started or if the timer
   * has already been deleted by deleteTimer() before the destructor is called.
   * This value can be updated by calling setDeleteBlockTime().
   *
   * <b>Example Usage</b>
   * @include Timer/timer.cpp
   */
  explicit Timer(const TickType_t period, const bool autoReload = false,
                 const char* name = "", const TickType_t deleteBlockTime = 0)
      : TimerBase(deleteBlockTime) {
    this->handle = xTimerCreate(name, period, (autoReload ? pdTRUE : pdFALSE),
                                this, callTimerFunction);
  }
  ~Timer() = default;

  Timer(Timer&&) noexcept = default;
  Timer& operator=(Timer&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * Timer Timer.hpp <FreeRTOS/Timer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS timer.
 *
 * Each software timer requires a small amount of RAM that is used to hold the
 * timer's state. If a timer is created using FreeRTOS::Timer() then this RAM is
 * automatically allocated from the FreeRTOS heap. If a software timer is
 * created this class then the RAM is included in the object so it can be
 * statically allocated at compile time. See the Static Vs Dynamic allocation
 * page for more information.
 *
 * @note This class is not intended to be instantiated by the user.  The user
 * should create a class that derives from this class and implement
 * timerFunction().
 *
 * @warning This class contains the timer data structure, so any instance of
 * this class or class derived from this class should be persistent (not
 * declared on the stack of another function).
 */
class StaticTimer : public TimerBase {
 public:
  StaticTimer(const StaticTimer&) = delete;
  StaticTimer& operator=(const StaticTimer&) = delete;

 protected:
  /**
   * Timer.hpp
   *
   * @brief Construct a new StaticTimer object by calling <tt>TimerHandle_t
   * xTimerCreateStatic( const char * const pcTimerName, const TickType_t
   * xTimerPeriod, const UBaseType_t uxAutoReload, void * const pvTimerID,
   * TimerCallbackFunction_t pxCallbackFunction StaticTimer_t *pxTimerBuffer
   * )</tt>
   *
   * @see <https://www.freertos.org/xTimerCreateStatic.html>
   *
   * @note When calling <tt>xTimerCreateStatic</tt> the constructor passes the
   * <tt>this</tt> pointer as the pvTimerID argument. This pointer is used so
   * that the interface function callTimerFunction() can invoke timerFunction()
   * for this instance of the class.
   *
   * @note Timers are created in the dormant state. The start(), reset(),
   * startFromISR(), resetFromISR(), changePeriod() and changePeriodFromISR()
   * API functions can all be used to transition a timer into the active state.
   *
   * @param period The period of the timer. The period is specified in ticks,
   * and the macro pdMS_TO_TICKS() can be used to convert a time specified in
   * milliseconds to a time specified in ticks. For example, if the timer must
   * expire after 100 ticks, then simply set period to 100. Alternatively, if
   * the timer must expire after 500ms, then set period to pdMS_TO_TICKS( 500 ).
   * pdMS_TO_TICKS() can only be used if configTICK_RATE_HZ is less than or
   * equal to 1000. The timer period must be greater than 0.
   * @param autoReload If autoReload is set to true, then the timer will expire
   * repeatedly with a frequency set by the period parameter. If autoReload is
   * set to false, then the timer will be a one-shot and enter the dormant state
   * after it expires.
   * @param name A human readable text name that is assigned to the timer.  This
   * is done purely to assist debugging. The RTOS kernel itself only ever
   * references a timer by its handle, and never by its name.
   * @param deleteBlockTime Specifies the time, in ticks, that the calling task
   * should be held in the Blocked state to wait for the delete command to be
   * successfully sent to the timer command queue, should the queue already be
   * full when the destructor is called. deleteBlockTime is ignored if the
   * destructor is called before the RTOS scheduler is started or if the timer
   * has already been deleted by deleteTimer() before the destructor is called.
   * This value can be updated by calling setDeleteBlockTime().
   *
   * <b>Example Usage</b>
   * @include Timer/staticTimer.cpp
   */
  explicit StaticTimer(const TickType_t period, const bool autoReload = false,
                       const char* name = "",
                       const TickType_t deleteBlockTime = 0)
      : TimerBase(deleteBlockTime) {
    this->handle =
        xTimerCreateStatic(name, period, (autoReload ? pdTRUE : pdFALSE), this,
                           callTimerFunction, &staticTimer);
  }
  ~StaticTimer() = default;

  StaticTimer(StaticTimer&&) noexcept = default;
  StaticTimer& operator=(StaticTimer&&) noexcept = default;

 private:
  StaticTimer_t staticTimer;
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

inline void callTimerFunction(TimerHandle_t timer) {
  static_cast<FreeRTOS::TimerBase*>(pvTimerGetTimerID(timer))->timerEntry();
}

#endif  // FREERTOS_TIMER_HPP
