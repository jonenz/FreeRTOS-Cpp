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

#ifndef FREERTOS_KERNEL_HPP
#define FREERTOS_KERNEL_HPP

#include "FreeRTOS.h"
#include "task.h"

namespace FreeRTOS {

/**
 * @brief Kernel namespace that provides an interface to kernel functions.
 *
 */
namespace Kernel {
enum class SchedulerState : BaseType_t {
  Suspended = taskSCHEDULER_SUSPENDED,
  NotStarted = taskSCHEDULER_NOT_STARTED,
  Running = taskSCHEDULER_RUNNING,
};

/**
 * @brief If versionNumber ends with + it represents the version in development
 * after the numbered release.
 */
inline constexpr char versionNumber[] = tskKERNEL_VERSION_NUMBER;
inline constexpr BaseType_t versionMajor = tskKERNEL_VERSION_MAJOR;
inline constexpr BaseType_t versionMinor = tskKERNEL_VERSION_MINOR;
inline constexpr BaseType_t versionBuild = tskKERNEL_VERSION_BUILD;

#if (INCLUDE_xTaskGetSchedulerState == 1)
/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskGetSchedulerState()</tt>
 *
 * @see <https://www.freertos.org/a00021.html#xTaskGetSchedulerState>
 *
 * @retval SchedulerState Returns the scheduler state as Running, NotStarted, or
 * Suspended.
 */
inline SchedulerState getSchedulerState() {
  return static_cast<SchedulerState>(xTaskGetSchedulerState());
}
#endif /* INCLUDE_xTaskGetSchedulerState */

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>uxTaskGetNumberOfTasks()</tt>
 *
 * @see <https://www.freertos.org/a00021.html#usTaskGetNumberOfTasks>
 *
 * @retval UBaseType_t The number of tasks that the real time kernel is
 * currently managing.  This includes all ready, blocked and suspended tasks.  A
 * task that has been deleted but not yet freed by the idle task will also be
 * included in the count.
 */
inline UBaseType_t getNumberOfTasks() { return uxTaskGetNumberOfTasks(); }

#if (INCLUDE_xTaskGetIdleTaskHandle == 1 && configGENERATE_RUN_TIME_STATS == 1)
/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskGetIdleRunTimeCounter()</tt>
 *
 * @see <https://www.freertos.org/a00021.html#vTaskGetIdleRunTimeCounter>
 *
 * @retval TickType_t The run-time counter for the Idle task.
 *
 * This function can be used to determine how much CPU time the idle task
 * receives.  See the Run Time Stats page for a full description of the
 * run-time-stats feature.
 *
 * configGENERATE_RUN_TIME_STATS and INCLUDE_xTaskGetIdleTaskHandle must both be
 * defined as 1 for this function to be available.  The application must also
 * then provide definitions for portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() and
 * portGET_RUN_TIME_COUNTER_VALUE to configure a peripheral timer/counter and
 * return the timer's current count value respectively.  It is recommended to
 * make the timer at least 10 times the frequency of the tick count.
 */
inline TickType_t getIdleRunTimeCounter() {
  return xTaskGetIdleRunTimeCounter();
}
#endif /* INCLUDE_xTaskGetIdleTaskHandle && configGENERATE_RUN_TIME_STATS*/

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskGetTickCount()</tt>
 *
 * @see <https://www.freertos.org/a00021.html#xTaskGetTickCount>
 *
 * @retval TickType_t The count of ticks since
 * FreeRTOS::Kernel::startScheduler() was called.
 */
inline TickType_t getTickCount() { return xTaskGetTickCount(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskGetTickCountFromISR()</tt>
 *
 * @see <https://www.freertos.org/a00021.html#xTaskGetTickCountFromISR>
 *
 * @retval TickType_t The count of ticks since
 * FreeRTOS::Kernel::startScheduler() was called.
 *
 * This is a version of FreeRTOS::Kernel::getTickCount() that is safe to be
 * called from an ISR - provided that TickType_t is the natural word size of the
 * microcontroller being used or interrupt nesting is either not supported or
 * not being used.
 */
inline TickType_t getTickCountFromISR() { return xTaskGetTickCountFromISR(); }

/**
 *Kernel.hpp
 *
 * @brief Function that calls <tt>taskYIELD()</tt>
 *
 * @see <https://www.freertos.org/a00020.html#taskYIELD>
 *
 * FreeRTOS::Kernel::yield() is used to request a context switch to another
 *task. However, if there are no other tasks at a higher or equal priority to
 *the task that calls FreeRTOS::Kernel::yield() then the RTOS scheduler will
 *simply select the task that called FreeRTOS::Kernel::yield() to run again.
 */
inline void yield() { taskYIELD(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskENTER_CRITICAL()</tt>
 *
 * @see <https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html>
 *
 * Function to mark the start of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation) so
 * must be used with care!
 *
 * <b>Example Usage</b>
 * @include Kernel/enterExitCritical.cpp
 */
inline void enterCritical() { taskENTER_CRITICAL(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskENTER_CRITICAL_FROM_ISR()</tt>
 *
 * @see
 * <https://www.freertos.org/taskENTER_CRITICAL_FROM_ISR_taskEXIT_CRITICAL_FROM_ISR.html>
 *
 * @retval uint32_t the interrupt mask state as it was before the macro was
 * called.  The value returned by FreeRTOS::Kernel::enterCriticalFromISR() must
 * be used as the interruptStatus parameter in the matching call to
 * FreeRTOS::Kernel::exitCriticalFromISR().
 *
 * Function to mark the start of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation) so
 * must be used with care!
 *
 * <b>Example Usage</b>
 * @include Kernel/enterExitCriticalFromISR.cpp
 */
inline uint32_t enterCriticalFromISR() { return taskENTER_CRITICAL_FROM_ISR(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskEXIT_CRITICAL()</tt>
 *
 * @see <https://www.freertos.org/taskENTER_CRITICAL_taskEXIT_CRITICAL.html>
 *
 * Function to mark the end of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation) so
 * must be used with care!
 *
 * <b>Example Usage</b>
 * @include Kernel/enterExitCritical.cpp
 */
inline void exitCritical() { taskEXIT_CRITICAL(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskEXIT_CRITICAL_FROM_ISR()</tt>
 *
 * @see
 * <https://www.freertos.org/taskENTER_CRITICAL_FROM_ISR_taskEXIT_CRITICAL_FROM_ISR.html>
 *
 * @param interruptStatus The value used as the interruptStatus parameter must
 * be the value returned from the matching call to
 * FreeRTOS::Kernel::enterCriticalFromISR().
 *
 * Function to mark the end of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation) so
 * must be used with care!
 *
 * <b>Example Usage</b>
 * @include Kernel/enterExitCriticalFromISR.cpp
 */
inline void exitCriticalFromISR(const uint32_t interruptStatus) {
  taskEXIT_CRITICAL_FROM_ISR(interruptStatus);
}

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskDISABLE_INTERRUPTS()</tt>
 *
 * @see <https://www.freertos.org/a00020.html#taskDISABLE_INTERRUPTS>
 *
 * Function to disable all maskable interrupts.
 */
inline void disableInterrupts() { taskDISABLE_INTERRUPTS(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>taskENABLE_INTERRUPTS()</tt>
 *
 * @see <https://www.freertos.org/a00020.html#taskENABLE_INTERRUPTS>
 *
 * Function to enable microcontroller interrupts.
 */
inline void enableInterrupts() { taskENABLE_INTERRUPTS(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>vTaskStartScheduler()</tt>
 *
 * @see <https://www.freertos.org/a00132.html>
 *
 * Starts the real time kernel tick processing.  After calling the kernel has
 * control over which tasks are executed and when.
 *
 * <b>Example Usage</b>
 * @include Kernel/startScheduler.cpp
 */
inline void startScheduler() { vTaskStartScheduler(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>vTaskEndScheduler()</tt>
 *
 * @see <https://www.freertos.org/a00133.html>
 *
 * @note At the time of writing only the x86 real mode port, which runs on a PC
 * in place of DOS, implements this function.
 *
 * Stops the real time kernel tick.  All created tasks will be automatically
 * deleted and multitasking (either preemptive or cooperative) will stop.
 * Execution then resumes from the point where
 * FreeRTOS::Kernel::startScheduler() was called, as if
 * FreeRTOS::Kernel::startScheduler() had just returned.
 *
 * See the demo application file main. c in the demo/PC directory for an example
 * that uses FreeRTOS::Kernel::endScheduler().
 *
 * FreeRTOS::Kernel::endScheduler() requires an exit function to be defined
 * within the portable layer (see vPortEndScheduler () in port. c for the PC
 * port).  This performs hardware specific operations such as stopping the
 * kernel tick.
 *
 * FreeRTOS::Kernel::endScheduler() will cause all of the resources allocated by
 * the kernel to be freed - but will not free resources allocated by application
 * tasks.
 *
 * <b>Example Usage</b>
 * @include Kernel/endScheduler.cpp
 */
inline void endScheduler() { vTaskEndScheduler(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>vTaskSuspendAll()</tt>
 *
 * @see <https://www.freertos.org/a00134.html>
 *
 * Suspends the scheduler without disabling interrupts.  Context switches will
 * not occur while the scheduler is suspended.
 *
 * After calling FreeRTOS::Kernel::suspendAll() the calling task will continue
 * to execute without risk of being swapped out until a call to
 * FreeRTOS::Kernel::resumeAll() has been made.
 *
 * API functions that have the potential to cause a context switch (for example,
 * FreeRTOS::Task::delayUntil(), FreeRTOS::Queue::send(), etc.) must not be
 * called while the scheduler is suspended.
 *
 * <b>Example Usage</b>
 * @include Kernel/suspendAll.cpp
 */
inline void suspendAll() { vTaskSuspendAll(); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskResumeAll()</tt>
 *
 * @see <https://www.freertos.org/a00135.html>
 *
 * Resumes scheduler activity after it was suspended by a call to
 * FreeRTOS::Kernel::suspendAll().
 *
 * FreeRTOS::Kernel::resumeAll() only resumes the scheduler.  It does not
 * unsuspend tasks that were previously suspended by a call to
 * FreeRTOS::Task::suspend().
 *
 * @retval true If resuming the scheduler caused a context switch.
 * @retval false Otherwise.
 *
 * <b>Example Usage</b>
 * @include Kernel/resumeAll.cpp
 */
inline bool resumeAll() { return (xTaskResumeAll() == pdTRUE); }

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>vTaskStepTick( const TickType_t xTicksToJump
 * )</tt>
 *
 * @see <https://www.freertos.org/vTaskStepTick.html>
 *
 * Only available when configUSE_TICKLESS_IDLE is set to 1.  If tickless mode is
 * being used, or a low power mode is implemented, then the tick interrupt will
 * not execute during idle periods.  When this is the case, the tick count value
 * maintained by the scheduler needs to be kept up to date with the actual
 * execution time by being skipped forward by a time equal to the idle period.
 *
 * @param ticksToJump The number of RTOS ticks that have passed since the tick
 * interrupt was stopped.
 */
inline void stepTick(const TickType_t ticksToJump) {
  vTaskStepTick(ticksToJump);
}

/**
 * Kernel.hpp
 *
 * @brief Function that calls <tt>xTaskCatchUpTicks( TickType_t xTicksToCatchUp
 * )</tt>
 *
 * @see <https://www.freertos.org/vTaskStepTick.html>
 *
 * This function corrects the tick count value after the application code has
 * held interrupts disabled for an extended period resulting in tick interrupts
 * having been missed.
 *
 * This function is similar to FreeRTOS::Kernel::stepTick(), however, unlike
 * FreeRTOS::Kernel::stepTick(), FreeRTOS::Kernel::catchUpTicks() may move the
 * tick count forward past a time at which a task should be removed from the
 * blocked state.  That means tasks may have to be removed from the blocked
 * state as the tick count is moved.
 *
 * @param ticksToCatchUp The number of tick interrupts that have been missed due
 * to interrupts being disabled.  Its value is not computed automatically, so
 * must be computed by the application writer.
 *
 * @retval true If moving the tick count forward resulted in a task leaving the
 * blocked state and a context switch being performed.
 * @retval false Otherwise.
 */
inline bool catchUpTicks(const TickType_t ticksToCatchUp) {
  return (xTaskCatchUpTicks(ticksToCatchUp) == pdTRUE);
}
}  // namespace Kernel

}  // namespace FreeRTOS

#endif  // FREERTOS_KERNEL_HPP
