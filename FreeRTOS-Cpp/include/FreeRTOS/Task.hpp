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

#ifndef FREERTOS_TASK_HPP
#define FREERTOS_TASK_HPP

#include <FreeRTOS/Kernel.hpp>
#include <bitset>
#include <utility>

#include "FreeRTOS.h"
#include "task.h"

/**
 * @brief C function that is used to interface this class with the FreeRTOS
 * kernel.
 *
 * @note This function should not be called or referenced by the user.
 *
 * @param task Pointer to an instance of FreeRTOS::TaskBase.
 */
void callTaskFunction(void* task);

namespace FreeRTOS {

/**
 * @class TaskBase Task.hpp <FreeRTOS/Task.hpp>
 *
 * @brief Base class that provides the standard task interface to FreeRTOS::Task
 * and FreeRTOS::StaticTask.
 *
 * @note This class is not intended to be instantiated or derived from by the
 * user. Use FreeRTOS::Task or FreeRTOS::StaticTask as a base class for a user
 * implemented task.
 */
class TaskBase {
 public:
  friend class Task;
  template <UBaseType_t>
  friend class StaticTask;

  TaskBase(const TaskBase&) = delete;
  TaskBase& operator=(const TaskBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  enum class State {
    Running = eRunning,
    Ready = eReady,
    Blocked = eBlocked,
    Suspended = eSuspended,
    Deleted = eDeleted,
    Invalid = eInvalid,
  };

  enum class NotifyAction {
    NoAction = eNoAction,
    SetBits = eSetBits,
    Increment = eIncrement,
    SetValueWithOverwrite = eSetValueWithOverwrite,
    SetValueWithoutOverwrite = eSetValueWithoutOverwrite,
  };

  // NOLINTNEXTLINE
  using NotificationBits = std::bitset<32>;

  /**
   * Task.hpp
   *
   * @brief Function that acts as the entry point of the task instance. This
   * function initializes the previous wake time of the task and calls the user
   * implemented taskFunction().
   *
   * @note This function is only public so that it can be accessed by the C
   * interface function <tt>callTaskFunction()</tt> and should not be called or
   * referenced by the user.
   */
  virtual inline void taskEntry() final {
    previousWakeTime = FreeRTOS::Kernel::getTickCount();
    taskFunction();
  };

#if (INCLUDE_uxTaskPriorityGet == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxTaskPriorityGet( TaskHandle_t
   * xTask )</tt>
   *
   * @see <https://www.freertos.org/a00128.html>
   *
   * INCLUDE_uxTaskPriorityGet must be defined as 1 for this function to be
   * available. See the RTOS Configuration documentation for more information.
   *
   * Obtain the priority of any task.
   *
   * @return UBaseType_t The priority of the task.
   *
   * <b>Example Usage</b>
   * @include Task/getPriority.cpp
   */
  inline UBaseType_t getPriority() const { return uxTaskPriorityGet(handle); }
#endif /* INCLUDE_uxTaskPriorityGet */

#if (INCLUDE_vTaskPrioritySet == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskPrioritySet( TaskHandle_t xTask,
   * UBaseType_t uxNewPriority )</tt>
   *
   * @see <https://www.freertos.org/a00129.html>
   *
   * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be
   * available. See the configuration section for more information.
   *
   * Set the priority of the task.
   *
   * A context switch will occur before the function returns if the priority
   * being set is higher than the currently executing task.
   *
   * @param newPriority The priority to which the task will be set.
   *
   * <b>Example Usage</b>
   * @include Task/setPriority.cpp
   */
  inline void setPriority(const UBaseType_t newPriority) const {
    vTaskPrioritySet(handle, newPriority);
  }
#endif /* INCLUDE_vTaskPrioritySet */

#if (INCLUDE_vTaskSuspend == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskSuspend( TaskHandle_t
   * xTaskToSuspend )</tt>
   *
   * @see <https://www.freertos.org/a00130.html>
   *
   * INCLUDE_vTaskSuspend must be defined as 1 for this function to be
   * available. See the RTOS Configuration documentation for more information.
   *
   * Suspend a task. When suspended a task will never get any microcontroller
   * processing time, no matter what its priority.
   *
   * Calls to suspend() are not accumulative - i.e. calling suspend() twice on
   * the same task still only requires one call to resume() to ready the
   * suspended task.
   *
   * <b>Example Usage</b>
   * @include Task/suspend.cpp
   */
  inline void suspend() const { vTaskSuspend(handle); }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskResume( TaskHandle_t xTaskToResume
   * )</tt>
   *
   * @see <https://www.freertos.org/a00131.html>
   *
   * INCLUDE_vTaskSuspend must be defined as 1 for this function to be
   * available. See the RTOS Configuration documentation for more information.
   *
   * Resumes a suspended task.
   *
   * A task that has been suspended by one or more calls to suspend() will be
   * made available for running again by a single call to resume().
   *
   * <b>Example Usage</b>
   * @include Task/resume.cpp
   */
  inline void resume() const { vTaskResume(handle); }

#if (INCLUDE_xTaskResumeFromISR == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskResumeFromISR( TaskHandle_t
   * xTaskToResume )</tt>
   *
   * @see <https://www.freertos.org/taskresumefromisr.html>
   *
   * INCLUDE_xTaskResumeFromISR must be defined as 1 for this function to be
   * available.  See the configuration section for more information.
   *
   * An implementation of resume() that can be called from within an ISR.
   *
   * A task that has been suspended by one or more calls to suspend() will be
   * made available for running again by a single call to resumeFromISR().
   *
   * resumeFromISR() should not be used to synchronize a task with an interrupt
   * if there is a chance that the interrupt could arrive prior to the task
   * being suspended - as this can lead to interrupts being missed. Use of a
   * semaphore as a synchronisation mechanism would avoid this eventuality.
   *
   * @retval true If resuming the task should result in a context switch.  This
   * is used by the ISR to determine if a context switch may be required
   * following the ISR.
   * @retval false Otherwise.
   *
   * <b>Example Usage</b>
   * @include Task/resumeFromISR.cpp
   */
  inline bool resumeFromISR() const {
    return (xTaskResumeFromISR(handle) == pdTRUE);
  }
#endif /* INCLUDE_xTaskResumeFromISR */
#endif /* INCLUDE_vTaskSuspend */

#if (INCLUDE_xTaskAbortDelay == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskAbortDelay( TaskHandle_t
   * xTask )</tt>
   *
   * @see <https://www.freertos.org/xTaskAbortDelay.html>
   *
   * INCLUDE_xTaskAbortDelay must be defined as 1 in FreeRTOSConfig.h for this
   * function to be available.
   *
   * A task will enter the Blocked state when it is waiting for an event.  The
   * event it is waiting for can be a temporal event (waiting for a time), such
   * as when delay() is called, or an event on an object, such as when
   * FreeRTOS::Queue::receive() or notifyTake() is called.  If the handle of a
   * task that is in the Blocked state is used in a call to abortDelay() then
   * the task will leave the Blocked state, and return from whichever function
   * call placed the task into the Blocked state.
   *
   * There is no 'FromISR' version of this function as an interrupt would need
   * to know which object a task was blocked on in order to know which actions
   * to take.  For example, if the task was blocked on a queue the interrupt
   * handler would then need to know if the queue was locked.
   *
   * @retval true Otherwise.
   * @retval false If the task was not in the Blocked state.
   */
  inline bool abortDelay() const { return (xTaskAbortDelay(handle) == pdPASS); }
#endif /* INCLUDE_xTaskAbortDelay */

#if (INCLUDE_xTaskGetIdleTaskHandle == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>TaskHandle_t xTaskGetIdleTaskHandle( void
   * )</tt>
   *
   * @see <https://www.freertos.org/a00021.html#xTaskGetIdleTaskHandle>
   *
   * getIdleTaskHandle() is only available if INCLUDE_xTaskGetIdleTaskHandle is
   * set to 1 in FreeRTOSConfig.h.
   *
   * Simply returns the handle of the idle task.  It is not valid to call
   * getIdleTaskHandle() before the scheduler has been started.
   *
   * @return TaskHandle_t The task handle associated with the Idle task. The
   * Idle task is created automatically when the RTOS scheduler is started.
   */
  inline static TaskHandle_t getIdleHandle() {
    return xTaskGetIdleTaskHandle();
  }
#endif /* INCLUDE_xTaskGetIdleTaskHandle */

#if (INCLUDE_uxTaskGetStackHighWaterMark == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxTaskGetStackHighWaterMark(
   * TaskHandle_t xTask )</tt>
   *
   * @see <https://www.freertos.org/uxTaskGetStackHighWaterMark.html>
   *
   * INCLUDE_uxTaskGetStackHighWaterMark must be set to 1 in FreeRTOSConfig.h
   * for this function to be available.
   *
   * Returns the high water mark of the stack.  That is, the minimum free stack
   * space there has been (in words, so on a 32 bit machine a value of 1 means 4
   * bytes) since the task started.  The smaller the returned number the closer
   * the task has come to overflowing its stack.
   *
   * getStackHighWaterMark() and getStackHighWaterMark2() are the same except
   * for their return type.  Using configSTACK_DEPTH_TYPE allows the user to
   * determine the return type.  It gets around the problem of the value
   * overflowing on 8-bit types without breaking backward compatibility for
   * applications that expect an 8-bit return type.
   *
   * @return BaseType_t The smallest amount of free stack space there has been
   * (in words, so actual spaces on the stack rather than bytes) since the task
   * was created.
   */
  inline UBaseType_t getStackHighWaterMark() const {
    return uxTaskGetStackHighWaterMark(handle);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxTaskGetStackHighWaterMark2(
   * TaskHandle_t xTask )</tt>
   *
   * @see <https://www.freertos.org/uxTaskGetStackHighWaterMark.html>
   *
   * Returns the high water mark of the stack.  That is, the minimum free stack
   * space there has been (in words, so on a 32 bit machine a value of 1 means 4
   * bytes) since the task started.  The smaller the returned number the closer
   * the task has come to overflowing its stack.
   *
   * getStackHighWaterMark() and getStackHighWaterMark2() are the same except
   * for their return type.  Using configSTACK_DEPTH_TYPE allows the user to
   * determine the return type.  It gets around the problem of the value
   * overflowing on 8-bit types without breaking backward compatibility for
   * applications that expect an 8-bit return type.
   *
   * @return configSTACK_DEPTH_TYPE The smallest amount of free stack space
   * there has been (in words, so actual spaces on the stack rather than bytes)
   * since the task was created.
   */
  inline configSTACK_DEPTH_TYPE getStackHighWaterMark2() const {
    return uxTaskGetStackHighWaterMark2(handle);
  }
#endif /* INCLUDE_uxTaskGetStackHighWaterMark */

#if (INCLUDE_eTaskGetState == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxTaskGetStackHighWaterMark2(
   * TaskHandle_t xTask )</tt>
   *
   * @see <https://www.freertos.org/a00021.html#eTaskGetState>
   *
   * @see getInfo()
   *
   * INCLUDE_eTaskGetState must be defined as 1 for this function to be
   * available.  See the configuration section for more information.
   *
   * Obtain the state of any task.  States are encoded by the
   * FreeRTOS::Task::State enumerated class type.
   *
   * @return State The state of the task at the time the function was called.
   * Note the state of the task might change between the function being called,
   * and the functions return value being tested by the calling task.
   */
  inline State getState() const {
    return static_cast<State>(eTaskGetState(handle));
  }
#endif /* INCLUDE_eTaskGetState */

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>char *pcTaskGetName( TaskHandle_t
   * xTaskToQuery )</tt>
   *
   * @see <https://www.freertos.org/a00021.html#pcTaskGetName>
   *
   * Looks up the name of a task.
   *
   * @return const char* The text (human readable) name of the task.  A pointer
   * to the subject task's name, which is a standard NULL terminated C string.
   */
  inline const char* getName() const { return pcTaskGetName(handle); }

#if (INCLUDE_xTaskGetHandle == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>TaskHandle_t xTaskGetHandle( const char
   * *pcNameToQuery )</tt>
   *
   * @see <https://www.freertos.org/a00021.html#xTaskGetHandle>
   *
   * @note This function takes a relatively long time to complete and should be
   * used sparingly.
   *
   * @param name The text name (as a standard C NULL terminated string) of the
   * task for which the handle will be returned.
   * @return TaskHandle_t The handle of the task that has the human readable
   * name.  NULL is returned if no matching name is found.
   * INCLUDE_xTaskGetHandle must be set to 1 in FreeRTOSConfig.h for getHandle()
   * to be available.
   */
  inline static TaskHandle_t getHandle(const char* name) {
    return xTaskGetHandle(name);
  }
#endif /* INCLUDE_xTaskGetHandle */

#if (configUSE_TASK_NOTIFICATIONS == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskNotifyGiveIndexed(
   * TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyGive.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each task has a private array of "notification values" (or
   * 'notifications'), each of which is a 32-bit unsigned integer (uint32_t).
   * The constant configTASK_NOTIFICATION_ARRAY_ENTRIES sets the number of
   * indexes in the array, and (for backward compatibility) defaults to 1 if
   * left undefined.  Prior to FreeRTOS V10.4.0 there was only one notification
   * value per task.
   *
   * Events can be sent to a task using an intermediary object.  Examples of
   * such objects are queues, semaphores, mutexes and event groups.  Task
   * notifications are a method of sending an event directly to a task without
   * the need for such an intermediary object.
   *
   * A notification sent to a task can optionally perform an action, such as
   * update, overwrite or increment one of the task's notification values.  In
   * that way task notifications can be used to send data to a task, or be used
   * as light weight and fast binary or counting semaphores.
   *
   * notifyGive() indicies is are intended for use when task notifications are
   * used as light weight and faster binary or counting semaphore equivalents.
   * Actual FreeRTOS semaphores are given using the FreeRTOS::Semaphore API, the
   * equivalent action that instead uses a task notification is notifyGive().
   *
   * When task notifications are being used as a binary or counting semaphore
   * equivalent then the task being notified should wait for the notification
   * using the notificationTake() API function rather than the
   * notifyWaitIndexed() API function.
   *
   * @note Each notification within the array operates independently - a task
   * can only block on one notification within the array at a time and will not
   * be unblocked by a notification sent to any other array index.
   *
   * @param index The index within the target task's array of notification
   * values to which the notification is to be sent.  index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * <b>Example Usage</b>
   * @include Task/notifyGive.cpp
   */
  inline void notifyGive(const UBaseType_t index = 0) const {
    xTaskNotifyGiveIndexed(handle, index);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskNotifyGiveIndexedFromISR(
   * TaskHandle_t xTaskHandle, UBaseType_t uxIndexToNotify, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/vTaskNotifyGiveFromISR.html>
   *
   * Version of notifyGive() that can be used from an interrupt service routine
   * (ISR). See the documentation page for the notifyGive() API function for a
   * description of their operation and the necessary configuration parameters.
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending the notification caused a task to unblock, and the unblocked task
   * has a priority higher than the currently running task.  If
   * higherPriorityTaskWoken is set to true, then a context switch should be
   * requested before the interrupt is exited.
   * @param index The index within the target task's array of notification
   * values to which the notification is to be sent.  index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * <b>Example Usage</b>
   * @include Task/notifyGiveFromISR.cpp
   */
  inline void notifyGiveFromISR(bool& higherPriorityTaskWoken,
                                const UBaseType_t index = 0) const {
    BaseType_t taskWoken = pdFALSE;
    vTaskNotifyGiveIndexedFromISR(handle, index, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskNotifyGiveIndexedFromISR(
   * TaskHandle_t xTaskHandle, UBaseType_t uxIndexToNotify, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/vTaskNotifyGiveFromISR.html>
   *
   * @overload
   */
  inline void notifyGiveFromISR(const UBaseType_t index = 0) const {
    vTaskNotifyGiveIndexedFromISR(handle, index, NULL);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskNotifyIndexed( TaskHandle_t
   * xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction
   * eAction )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotify.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each task has a private array of "notification values" (or
   * 'notifications'), each of which is a 32-bit unsigned integer (uint32_t).
   * The constant configTASK_NOTIFICATION_ARRAY_ENTRIES sets the number of
   * indexes in the array, and (for backward compatibility) defaults to 1 if
   * left undefined.  Prior to FreeRTOS V10.4.0 there was only one notification
   * value per task.
   *
   * Events can be sent to a task using an intermediary object.  Examples of
   * such objects are queues, semaphores, mutexes and event groups.  Task
   * notifications are a method of sending an event directly to a task without
   * the need for such an intermediary object.
   *
   * A notification sent to a task can optionally perform an action, such as
   * update, overwrite or increment one of the task's notification values.  In
   * that way task notifications can be used to send data to a task, or be used
   * as light weight and fast binary or counting semaphores.
   *
   * A task can use notifyWait() or notifyTake() to [optionally] block to wait
   * for a notification to be pending.  The task does not consume any CPU time
   * while it is in the Blocked state.
   *
   * A notification sent to a task will remain pending until it is cleared by
   * the task calling notifyWait() or notifyTake().  If the task was already in
   * the Blocked state to wait for a notification when the notification arrives
   * then the task will automatically be removed from the Blocked state
   * (unblocked) and the notification cleared.
   *
   * @note Each notification within the array operates independently - a task
   * can only block on one notification within the array at a time and will not
   * be unblocked by a notification sent to any other array index.
   *
   * @param action Specifies how the notification updates the task's
   * notification value, if at all.  Valid values for action are as follows:
   * <table> <tr><th>NotifyAction Setting     <th>Action Performed
   * <tr><td>NoAction                 <td>The target task receives the event,
   * but its notification value is not updated. In this case value is not used.
   * <tr><td>SetBits                  <td>The notification value of the target
   * task will be bitwise ORed with value. For example, if value is set to 0x01,
   * then bit 0 will get set within the target task's notification value.
   * Likewise if value is 0x04 then bit 2 will get set in the target task's
   * notification value. In this way the RTOS task notification mechanism can be
   * used as a light weight alternative to an event group. <tr><td>Increment
   * <td>The notification value of the target task will be incremented by one,
   * making the call to notify() equivalent to a call to notifyGive(). In this
   * case value is not used. <tr><td>SetValueWithOverwrite    <td>The
   * notification value of the target task is unconditionally set to value. In
   * this way the RTOS task notification mechanism is being used as a light
   * weight alternative to FreeRTOS::Queue::Overwrite().
   * <tr><td>SetValueWithoutOverwrite <td>If the target task does not already
   * have a notification pending then its notification value will be set to
   * value.  If the target task already has a notification pending then its
   * notification value is not updated as to do so would overwrite the previous
   * value before it was used. In this case the call to notify() fails and false
   * is returned.  In this way the RTOS task notification mechanism is being
   * used as a light weight alternative to FreeRTOS::Queue::send() on a queue of
   * length 1.
   * @param value Data that can be sent with the notification.  How the data is
   * used depends on the value of the action parameter.
   * @param index The index within the target task's array of notification
   * values to which the notification is to be sent.  index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @return false If action is set to SetValueWithoutOverwrite and the task's
   * notification value cannot be updated because the target task already had a
   * notification pending.
   * @return true Otherwise.
   *
   * <b>Example Usage</b>
   * @include Task/notify.cpp
   */
  inline bool notify(const NotifyAction action,
                     const NotificationBits value = 0,
                     const UBaseType_t index = 0) const {
    return (xTaskNotifyIndexed(handle, index, value.to_ulong(),
                               static_cast<eNotifyAction>(action)) == pdPASS);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskNotifyAndQueryIndexed(
   * TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue,
   * eNotifyAction eAction, uint32_t *pulPreviousNotifyValue )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyAndQuery.html>
   *
   * notifyAndQuery() performs the same operation as notify() with the addition
   * that it also returns the target task's prior notification value (the
   * notification value at the time the function is called rather than when the
   * function returns).
   *
   * @param action An enumerated type that can take one of the values documented
   * in the table below in order to perform the associated action. <table>
   * <tr><th>NotifyAction Setting     <th>Action Performed
   * <tr><td>NoAction                 <td>The target task receives the event,
   * but its notification value is not updated. In this case value is not used.
   * <tr><td>SetBits                  <td>The notification value of the target
   * task will be bitwise ORed with value. For example, if value is set to 0x01,
   * then bit 0 will get set within the target task's notification value.
   * Likewise if value is 0x04 then bit 2 will get set in the target task's
   * notification value. In this way the RTOS task notification mechanism can be
   * used as a light weight alternative to an event group. <tr><td>Increment
   * <td>The notification value of the target task will be incremented by one,
   * making the call to notify() equivalent to a call to notifyGive(). In this
   * case value is not used. <tr><td>SetValueWithOverwrite    <td>The
   * notification value of the target task is unconditionally set to value. In
   * this way the RTOS task notification mechanism is being used as a light
   * weight alternative to FreeRTOS::Queue::Overwrite().
   * <tr><td>SetValueWithoutOverwrite <td>If the target task does not already
   * have a notification pending then its notification value will be set to
   * value.  If the target task already has a notification pending then its
   * notification value is not updated as to do so would overwrite the previous
   * value before it was used. In this case the call to notify() fails and false
   * is returned.  In this way the RTOS task notification mechanism is being
   * used as a light weight alternative to FreeRTOS::Queue::send() on a queue of
   * length 1.
   * @param value Used to update the notification value of the task. See the
   * description of the action parameter below.
   * @param index The index within the target task's array of notification
   * values to which the notification is to be sent.  index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @return false If action is set to SetValueWithoutOverwrite and the task's
   * notification value cannot be updated because the target task already had a
   * notification pending.
   * @return true Otherwise.
   *
   * <b>Example Usage</b>
   * @include Task/notifyAndQuery.cpp
   */
  inline std::pair<bool, NotificationBits> notifyAndQuery(
      const NotifyAction action, const NotificationBits value = 0,
      const UBaseType_t index = 0) const {
    uint32_t pulNotificationValue;
    bool result =
        (xTaskNotifyAndQueryIndexed(handle, index, value.to_ulong(),
                                    static_cast<eNotifyAction>(action),
                                    &pulNotificationValue) == pdPASS);

    return std::make_pair(result, NotificationBits(pulNotificationValue));
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t
   xTaskNotifyAndQueryIndexedFromISR( TaskHandle_t xTaskToNotify,
   * UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction,
   uint32_t *pulPreviousNotifyValue,
   * BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyAndQueryFromISR.html>
   *
   * notifyAndQueryFromISR() performs the same operation as notifyFromISR() with
   the addition that it also returns the
   * target task's prior notification value (the notification value at the time
   the function is called rather than at
   * the time the function returns).

   * @param higherPriorityTaskWoken A reference that will be set to true if
   sending the notification caused a task to
   * unblock, and the unblocked task has a priority higher than the currently
   running task.  If higherPriorityTaskWoken
   * is set to true, then a context switch should be requested before the
   interrupt is exited.
   * @param action An enumerated type that can take one of the values documented
   in the table below in order to perform
   * the associated action. <table>
   * <tr><th>NotifyAction Setting     <th>Action Performed
   * <tr><td>NoAction                 <td>The target task receives the event,
   but its notification value is not updated.
   * In this case value is not used.
   * <tr><td>SetBits                  <td>The notification value of the target
   task will be bitwise ORed with value. For
   * example, if value is set to 0x01, then bit 0 will get set within the target
   task's notification value. Likewise if
   * value is 0x04 then bit 2 will get set in the target task's notification
   value. In this way the RTOS task
   * notification mechanism can be used as a light weight alternative to an
   event group.
   * <tr><td>Increment                <td>The notification value of the target
   task will be incremented by one, making
   * the call to notify() equivalent to a call to notifyGive(). In this case
   value is not used.
   * <tr><td>SetValueWithOverwrite    <td>The notification value of the target
   task is unconditionally set to value. In
   * this way the RTOS task notification mechanism is being used as a light
   weight alternative to
   * FreeRTOS::Queue::Overwrite().
   * <tr><td>SetValueWithoutOverwrite <td>If the target task does not already
   have a notification pending then its
   * notification value will be set to value.  If the target task already has a
   notification pending then its
   * notification value is not updated as to do so would overwrite the previous
   value before it was used. In this case
   * the call to notify() fails and false is returned.  In this way the RTOS
   task notification mechanism is being used
   * as a light weight alternative to FreeRTOS::Queue::send() on a queue of
   length 1.
   * @param value Used to update the notification value of the task. See the
   description of the action parameter below.
   * @param index The index within the target task's array of notification
   values to which the notification is to be
   * sent.  index must be less than configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @retval false If action is set to SetValueWithoutOverwrite and the task's
   notification value cannot be updated
   * because the target task already had a notification pending.
   * @retval true Otherwise.
   * @return NotificationBits The task's notification value before any bits are
   modified.
   *
   * <b>Example Usage</b>
   * @include Task/notifyAndQueryFromISR.cpp
   */
  inline std::pair<bool, NotificationBits> notifyAndQueryFromISR(
      bool& higherPriorityTaskWoken, const NotifyAction action,
      const NotificationBits value = 0, const UBaseType_t index = 0) const {
    BaseType_t taskWoken = pdFALSE;
    uint32_t pulNotificationValue;
    bool result = (xTaskNotifyAndQueryIndexedFromISR(
                       handle, index, value.to_ulong(),
                       static_cast<eNotifyAction>(action),
                       &pulNotificationValue, &taskWoken) == pdPASS);

    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }

    return std::make_pair(result, NotificationBits(pulNotificationValue));
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t
   xTaskNotifyAndQueryIndexedFromISR( TaskHandle_t xTaskToNotify,
   * UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction,
   uint32_t *pulPreviousNotifyValue,
   * BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyAndQueryFromISR.html>
   *
   * @overload
   */
  inline std::pair<bool, NotificationBits> notifyAndQueryFromISR(
      const NotifyAction action, const NotificationBits value = 0,
      const UBaseType_t index = 0) const {
    uint32_t pulNotificationValue;
    bool result = (xTaskNotifyAndQueryIndexedFromISR(
                       handle, index, value.to_ulong(),
                       static_cast<eNotifyAction>(action),
                       &pulNotificationValue, NULL) == pdPASS);

    return std::make_pair(result, NotificationBits(pulNotificationValue));
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt> BaseType_t xTaskNotifyIndexedFromISR(
   * TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue,
   * eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyFromISR.html>
   *
   * Version of notify() that can be used from an interrupt service routine
   * (ISR). See the documentation page for the notify() API function for a
   * description of their operation and the necessary configuration parameters,
   * as well as backward compatibility information.
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending the notification caused a task to unblock, and the unblocked task
   * has a priority higher than the currently running task.  If
   * higherPriorityTaskWoken is set to true, then a context switch should be
   * requested before the interrupt is exited.
   * @param action An enumerated type that can take one of the values documented
   * in the table below in order to perform the associated action. <table>
   * <tr><th>NotifyAction Setting     <th>Action Performed
   * <tr><td>NoAction                 <td>The target task receives the event,
   * but its notification value is not updated. In this case value is not used.
   * <tr><td>SetBits                  <td>The notification value of the target
   * task will be bitwise ORed with value. For example, if value is set to 0x01,
   * then bit 0 will get set within the target task's notification value.
   * Likewise if value is 0x04 then bit 2 will get set in the target task's
   * notification value. In this way the RTOS task notification mechanism can be
   * used as a light weight alternative to an event group. <tr><td>Increment
   * <td>The notification value of the target task will be incremented by one,
   * making the call to notify() equivalent to a call to notifyGive(). In this
   * case value is not used. <tr><td>SetValueWithOverwrite    <td>The
   * notification value of the target task is unconditionally set to value. In
   * this way the RTOS task notification mechanism is being used as a light
   * weight alternative to FreeRTOS::Queue::Overwrite().
   * <tr><td>SetValueWithoutOverwrite <td>If the target task does not already
   * have a notification pending then its notification value will be set to
   * value.  If the target task already has a notification pending then its
   * notification value is not updated as to do so would overwrite the previous
   * value before it was used. In this case the call to notify() fails and false
   * is returned.  In this way the RTOS task notification mechanism is being
   * used as a light weight alternative to FreeRTOS::Queue::send() on a queue of
   * length 1.
   * @param value Used to update the notification value of the task. See the
   * description of the action parameter below.
   * @param index The index within the target task's array of notification
   * values to which the notification is to be sent.  index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @retval false If action is set to SetValueWithoutOverwrite and the task's
   * notification value cannot be updated because the target task already had a
   * notification pending.
   * @retval true Otherwise.
   *
   * <b>Example Usage</b>
   * This example demonstrates how to use notifyFromISR() with the SetBits
   * action. See the notify() API documentation page for examples showing how to
   * use the NoAction, SetValueWithOverwrite and SetValueWithoutOverwrite
   * actions.
   * @include Task/notifyFromISR.cpp
   */
  inline bool notifyFromISR(bool& higherPriorityTaskWoken,
                            const NotifyAction action,
                            const NotificationBits value = 0,
                            const UBaseType_t index = 0) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xTaskNotifyIndexedFromISR(handle, index, value.to_ulong(),
                                             static_cast<eNotifyAction>(action),
                                             &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt> BaseType_t xTaskNotifyIndexedFromISR(
   * TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue,
   * eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyFromISR.html>
   *
   * @overload
   */
  inline bool notifyFromISR(const NotifyAction action,
                            const NotificationBits value = 0,
                            const UBaseType_t index = 0) const {
    return (xTaskNotifyIndexedFromISR(handle, index, value.to_ulong(),
                                      static_cast<eNotifyAction>(action),
                                      NULL) == pdPASS);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskNotifyWaitIndexed(
   * UBaseType_t uxIndexToWaitOn, uint32_t ulBitsToClearOnEntry, uint32_t
   * ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t
   * xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyWait.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each task has an array of 'task notifications' (or just 'notifications'),
   * each of which has a state and a 32-bit value. A direct to task notification
   * is an event sent directly to a task that can unblock the receiving task,
   * and optionally update one of the receiving task’s notification values in a
   * number of different ways. For example, a notification may overwrite one of
   * the receiving task’s notification values, or just set one or more bits in
   * one of the receiving task’s notification values.
   *
   * notifyWait() waits, with an optional timeout, for the calling task to
   * receive a notification. If the receiving RTOS task was already Blocked
   * waiting for a notification when the notification it is waiting for arrives
   * the receiving RTOS task will be removed from the Blocked state and the
   * notification cleared.
   *
   * @note Each notification within the array operates independently - a task
   * can only block on one notification within the array at a time and will not
   * be unblocked by a notification sent to any other array index.
   *
   * notifyGive() must not be called from an interrupt service routine. Use
   * notifyGiveFromISR() instead.
   *
   * @param ticksToWait The maximum time to wait in the Blocked state for a
   * notification to be received if a notification is not already pending when
   * notifyWait() is called.
   *
   * The RTOS task does not consume any CPU time when it is in the Blocked
   * state.
   *
   * The time is specified in RTOS tick periods. The pdMS_TO_TICKS() macro can
   * be used to convert a time specified in milliseconds into a time specified
   * in ticks.
   *
   * @param bitsToClearOnEntry Any bits set in bitsToClearOnEntry will be
   * cleared in the calling RTOS task's notification value on entry to the
   * notifyWait() function (before the task waits for a new notification)
   * provided a notification is not already pending when notifyWait() is called.
   *
   * For example, if bitsToClearOnEntry is 0x01, then bit 0 of the task's
   * notification value will be cleared on entry to the function.
   *
   * Setting bitsToClearOnEntry to 0xffffffff (ULONG_MAX) will clear all the
   * bits in the task's notification value, effectively clearing the value to 0.
   *
   * @param bitsToClearOnExit Any bits set in bitsToClearOnExit will be cleared
   * in the calling RTOS task's notification value before notifyWait() function
   * exits if a notification was received.
   *
   * The bits are cleared after the RTOS task's notification value are returned.
   *
   * For example, if bitsToClearOnExit is 0x03, then bit 0 and bit 1 of the
   * task's notification value will be cleared before the function exits.
   *
   * Setting bitsToClearOnExit to 0xffffffff (ULONG_MAX) will clear all the bits
   * in the task's notification value, effectively clearing the value to 0.
   *
   * @param index The index within the calling task's array of notification
   * values on which the calling task will wait for a notification to be
   * received. index must be less than configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @retval true If a notification was received, or a notification was already
   * pending when notifyWait() was called.
   * @retval false If the call to notifyWait() timed out before a notification
   * was received.
   * @return The RTOS task's notification value as it was before any bits were
   * cleared due to the bitsToClearOnExit setting.
   *
   * <b>Example Usage</b>
   * @include Task/notifyWait.cpp
   */
  inline static std::pair<bool, NotificationBits> notifyWait(
      const TickType_t ticksToWait = portMAX_DELAY,
      const NotificationBits bitsToClearOnEntry = 0,
      const NotificationBits bitsToClearOnExit = 0,
      const UBaseType_t index = 0) {
    uint32_t pulNotificationValue;
    bool result =
        (xTaskNotifyWaitIndexed(index, bitsToClearOnEntry.to_ulong(),
                                bitsToClearOnExit.to_ulong(),
                                &pulNotificationValue, ticksToWait) == pdTRUE);
    return std::make_pair(result, NotificationBits(pulNotificationValue));
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskNotifyStateClearIndexed(
   * TaskHandle_t xTask, UBaseType_t uxIndexToClear )</tt>
   *
   * @see <https://www.freertos.org/xTaskNotifyStateClear.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each RTOS task has an array of task notifications. Each task notification
   * has a notification state that can be either ‘pending’ or ‘not pending’, and
   * a 32-bit notification value.
   *
   * If a notification is sent to an index within the array of notifications
   * then the notification at that index is said to be 'pending' until the task
   * reads its notification value or explicitly clears the notification state to
   * 'not pending' by calling notifyStateClear().
   *
   * @param index The index within the target task's array of notification
   * values to act upon. For example, setting index to 1 will clear the state of
   * the notification at index 1 within the array.
   *
   * index must be less than configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * notifyStateClear() does not have this parameter and always acts on the
   * notification at index 0.
   *
   * @retval true If the task had a notification pending, and the notification
   * was cleared.
   * @retval false If the task didn't have a notification pending.
   *
   * <b>Example Usage</b>
   * @include Task/notifyStateClear.cpp
   */
  inline bool notifyStateClear(const UBaseType_t index = 0) const {
    return (xTaskNotifyStateClearIndexed(handle, index) == pdTRUE);
  }

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>uint32_t ulTaskNotifyValueClearIndexed(
   * TaskHandle_t xTask, UBaseType_t uxIndexToClear, uint32_t ulBitsToClear
   * )</tt>
   *
   * @see <https://www.freertos.org/ulTasknotifyValueClear.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each RTOS task has an array of task notifications. Each task notification
   * has a notification state that can be either ‘pending’ or ‘not pending’, and
   * a 32-bit notification value.
   *
   * notifyValueClear() clears the bits specified by the bitsToClear bit mask in
   * the notification value at array index index of the task.
   *
   * @param bitsToClear Bit mask of the bits to clear in the notification value
   * of the task. Set a bit to 1 to clear the corresponding bits in the task's
   * notification value. Set bitsToClear to 0xffffffff (UINT_MAX on 32-bit
   * architectures) to clear the notification value to 0. Set bitsToClear to 0
   * to query the task's notification value without clearing any bits.
   * @param index The index within the target task's array of notification
   * values in which to clear the bits. index must be less than
   * configTASK_NOTIFICATION_ARRAY_ENTRIES.
   * @return NotificationBits The value of the target task's notification value
   * before the bits specified by bitsToClear were cleared.
   *
   * <b>Example Usage</b>
   * @include Task/notifyValueClear.cpp
   */
  inline NotificationBits notifyValueClear(
      const NotificationBits bitsToClear = 0,
      const UBaseType_t index = 0) const {
    return NotificationBits(
        ulTaskNotifyValueClearIndexed(handle, index, bitsToClear.to_ulong()));
  }
#endif /* configUSE_TASK_NOTIFICATIONS */

 protected:
  /**
   * Task.hpp
   *
   * @brief Abstraction function that acts as the entry point of the task for
   * the user.
   */
  virtual void taskFunction() = 0;

#if (INCLUDE_vTaskDelay == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>void vTaskDelay( const TickType_t
   * xTicksToDelay )</tt>
   *
   * @see <https://www.freertos.org/a00127.html>
   *
   * INCLUDE_vTaskDelay must be defined as 1 for this function to be available.
   * See the configuration section for more information.
   *
   * Delay a task for a given number of ticks.  The actual time that the task
   * remains blocked depends on the tick rate. The constant portTICK_PERIOD_MS
   * can be used to calculate real time from the tick rate - with the resolution
   * of one tick period.
   *
   * delay() specifies a time at which the task wishes to unblock relative to
   * the time at which delay() is called.  For example, specifying a block
   * period of 100 ticks will cause the task to unblock 100 ticks after delay()
   * is called. delay() does not therefore provide a good method of controlling
   * the frequency of a periodic task as the path taken through the code, as
   * well as other task and interrupt activity, will affect the frequency at
   * which delay() gets called and therefore the time at which the task next
   * executes.  See delayUntil() for an alternative API function designed to
   * facilitate fixed frequency execution.  It does this by specifying an
   * absolute time (rather than a relative time) at which the calling task
   * should unblock.
   *
   * @param ticksToDelay The amount of time, in tick periods, that the task
   * should block.
   *
   * <b>Example Usage</b>
   * @include Task/delay.cpp
   */
  inline static void delay(const TickType_t ticksToDelay = 0) {
    vTaskDelay(ticksToDelay);
  }
#endif /* INCLUDE_vTaskDelay */

#if (INCLUDE_xTaskDelayUntil == 1)
  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>BaseType_t xTaskDelayUntil( TickType_t
   * *pxPreviousWakeTime, const TickType_t xTimeIncrement )</tt>
   *
   * @see <https://www.freertos.org/xtaskdelayuntiltask-control.html>
   *
   * INCLUDE_xTaskDelayUntil must be defined as 1 for this function to be
   * available.  See the configuration section for more information.
   *
   * Delay a task until a specified time.  This function can be used by periodic
   * tasks to ensure a constant execution frequency.
   *
   * This function differs from delay() in one important aspect:  delay() will
   * cause a task to block for the specified number of ticks from the time delay
   * () is called.  It is therefore difficult to use delay() by itself to
   * generate a fixed execution frequency as the time between a task starting to
   * execute and that task calling delay() may not be fixed [the task may take a
   * different path though the code between calls, or may get interrupted or
   * preempted a different number of times each time it executes].
   *
   * Whereas delay() specifies a wake time relative to the time at which the
   * function is called, delayUntil() specifies the absolute (exact) time at
   * which it wishes to unblock.
   *
   * The function pdMS_TO_TICKS() can be used to calculate the number of ticks
   * from a time specified in milliseconds with a resolution of one tick period.
   *
   * @param timeIncrement The cycle time period. The task will be unblocked at
   * time (previousWakeTime + timeIncrement). Calling delayUntil() with the same
   * timeIncrement parameter value will cause the task to execute with a fixed
   * interval period.
   * @return true If the task way delayed.
   * @return false Otherwise.  A task will not be delayed if the next expected
   * wake time is in the past.
   *
   * <b>Example Usage</b>
   * @include Task/delayUntil.cpp
   */
  inline bool delayUntil(const TickType_t timeIncrement = 0) {
    return (xTaskDelayUntil(&previousWakeTime, timeIncrement) == pdTRUE);
  }
#endif /* INCLUDE_xTaskDelayUntil */

  /**
   * Task.hpp
   *
   * @brief Function that calls <tt>uint32_t ulTaskNotifyTakeIndexed(
   * UBaseType_t uxIndexToWaitOn, BaseType_t xClearCountOnExit, TickType_t
   * xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/ulTaskNotifyTake.html>
   *
   * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for these
   * functions to be available.
   *
   * Each task has a private array of "notification values" (or
   * 'notifications'), each of which is a 32-bit unsigned integer (uint32_t).
   * The constant configTASK_NOTIFICATION_ARRAY_ENTRIES sets the number of
   * indexes in the array, and (for backward compatibility) defaults to 1 if
   * left undefined.  Prior to FreeRTOS V10.4.0 there was only one notification
   * value per task.
   *
   * Events can be sent to a task using an intermediary object.  Examples of
   * such objects are queues, semaphores, mutexes and event groups.  Task
   * notifications are a method of sending an event directly to a task without
   * the need for such an intermediary object.
   *
   * A notification sent to a task can optionally perform an action, such as
   * update, overwrite or increment one of the task's notification values.  In
   * that way task notifications can be used to send data to a task, or be used
   * as light weight and fast binary or counting semaphores.
   *
   * notifyTake() is intended for use when a task notification is used as a
   * faster and lighter weight binary or counting semaphore alternative.  Actual
   * FreeRTOS semaphores are taken using the FreeRTOS::Semaphore::take() API
   * function, the equivalent action that instead uses a task notification is
   * notifyTake().
   *
   * When task notifications are being used as a binary or counting semaphore
   * equivalent then the task being notified should wait for the notification
   * using the notificationTake() API function rather than the notifyWait() API
   * function.
   *
   * notifyTake() can either clear the task's notification value at the array
   * index specified by the indexToWaitOn parameter to zero on exit, in which
   * case the notification value acts like a binary semaphore, or decrement the
   * notification value on exit, in which case the notification value acts like
   * a counting semaphore.
   *
   * A task can use notifyTake() to [optionally] block to wait for a
   * notification.  The task does not consume any CPU time while it is in the
   * Blocked state.
   *
   * Where as notifyWait() will return when a notification is pending,
   * notifyTake() will return when the task's notification value is not zero.
   *
   * @note Each notification within the array operates independently - a task
   * can only block on one notification within the array at a time and will not
   * be unblocked by a notification sent to any other array index.
   *
   * @param ticksToWait The maximum time to wait in the Blocked state for a
   * notification to be received if a notification is not already pending when
   * notifyTake() is called.  The RTOS task does not consume any CPU time when
   * it is in the Blocked state.  The time is specified in RTOS tick periods.
   * The pdMS_TO_TICKS() macro can be used to convert a time specified in
   * milliseconds into a time specified in ticks.
   *
   * @param clearCountOnExit If an RTOS task notification is received and
   * clearCountOnExit is set to false then the RTOS task's notification value is
   * decremented before notifyTake() exits. This is equivalent to the value of a
   * counting semaphore being decremented by a successful call to
   * FreeRTOS::Semaphore::Take(). If an RTOS task notification is received and
   * clearCountOnExit is set to true then the RTOS task's notification value is
   * reset to 0 before notifyTake() exits. This is equivalent to the value of a
   * binary semaphore being left at zero (or empty, or 'not available') after a
   * successful call to FreeRTOS::Semaphore::Take().
   *
   * @param index The index within the calling task's array of notification
   * values on which the calling task will wait for a notification to be
   * non-zero. index must be less than configTASK_NOTIFICATION_ARRAY_ENTRIES.
   *
   * @return NotificationBits The value of the task's notification value before
   * it is decremented or cleared (see the description of clearCountOnExit)
   *
   * <b>Example Usage</b>
   * @include Task/notifyTake.cpp
   */
  inline static NotificationBits notifyTake(
      const TickType_t ticksToWait = portMAX_DELAY,
      const bool clearCountOnExit = true, const UBaseType_t index = 0) {
    return NotificationBits(
        ulTaskNotifyTakeIndexed(index, clearCountOnExit, ticksToWait));
  }

 private:
  /**
   * @brief Construct a new TaskBase object.  This default constructor is
   * deliberately private as this class is not intended to be instantiated or
   * derived from by the user.  Use FreeRTOS::Task or FreeRTOS::StaticTask as a
   * base class for creating a task.
   */
  TaskBase() = default;

  TaskBase(TaskBase&&) noexcept = default;
  TaskBase& operator=(TaskBase&&) noexcept = default;

  /**
   * Task.hpp
   *
   * @brief Destroy the Task object.
   *
   * @see <https://www.freertos.org/a00126.html>
   *
   * If INCLUDE_vTaskDelete is defined as 1 and the task handle is not NULL,
   * then the destructor will call <tt>void vTaskDelete( TaskHandle_t xTask
   * )</tt>  See the RTOS Configuration documentation for more information.
   *
   * Calling <tt>void vTaskDelete( TaskHandle_t xTask )</tt> will remove a task
   * from the RTOS kernels management.  The task being deleted will be removed
   * from all ready, blocked, suspended and event lists.
   *
   * @note The idle task is responsible for freeing the RTOS kernel allocated
   * memory from tasks that have been deleted. It is therefore important that
   * the idle task is not starved of microcontroller processing time if your
   * application makes any calls to <tt>void vTaskDelete( TaskHandle_t xTask
   * )</tt> Memory allocated by the task code is not automatically freed, and
   * should be freed before the task is deleted.
   *
   * <b>Example Usage</b>
   * @include Task/task.cpp
   */
  ~TaskBase() {
#if (INCLUDE_vTaskDelete == 1)

    if (handle != NULL) {
      vTaskDelete(handle);
    }

#endif /* INCLUDE_vTaskDelete */
  }

  /**
   * @brief Handle used to refer to the task when using the FreeRTOS interface.
   */
  TaskHandle_t handle = NULL;

  /**
   * @brief Variable that holds the time at which the task was last unblocked.
   */
  TickType_t previousWakeTime = 0;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * Task Task.hpp <FreeRTOS/Task.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS task.
 *
 * Each task requires RAM that is used to hold the task state, and used by the
 * task as its stack. If a task is created using this class then the required
 * RAM is automatically allocated from the FreeRTOS heap. If a task is created
 * using FreeRTOS::StaticTask() then the RAM is included in the object, so it
 * can be statically allocated at compile time. See the Static Vs Dynamic
 * allocation page for more information.
 *
 * @note This class is not intended to be instantiated by the user.  The user
 * should create a class that derives from this class and implement
 * taskFunction().
 */
class Task : public TaskBase {
 public:
  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  /**
   * @brief Function that checks the return value of the call to xTaskCreate in
   * the constructor.  This function should be called to ensure the task was
   * created successfully before starting the scheduler.
   *
   * @return true If the task was created successfully.
   * @return false If the task was not created successfully due to insufficient
   * memory.
   */
  bool isValid() const { return taskCreatedSuccessfully; }

 protected:
  /**
   * Task.hpp
   *
   * @brief Construct a new Task object by calling <tt>BaseType_t xTaskCreate(
   * TaskFunction_t pvTaskCode, const char * const pcName,
   * configSTACK_DEPTH_TYPE usStackDepth, void *pvParameters, BaseType_t
   * uxPriority, TaskHandle_t *pxCreatedTask )</tt>
   *
   * @see <https://www.freertos.org/a00125.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * task was created successfully in case the memory required to create the
   * task could not be allocated.
   *
   * @note When calling <tt>xTaskCreate</tt> the constructor passes the
   * <tt>this</tt> pointer as the task function argument. This pointer is used
   * so that the interface function callTaskFunction() can invoke taskEntry()
   * for this instance of the class.
   *
   * @param priority The priority at which the created task will execute.
   * Priorities are asserted to be less than configMAX_PRIORITIES. If
   * configASSERT is undefined, priorities are silently capped at
   * (configMAX_PRIORITIES - 1).
   * @param stackDepth The number of words (not bytes!) to allocate for use as
   * the task's stack.  For example, if the stack is 16-bits wide and stackDepth
   * is 100, then 200 bytes will be allocated for use as the task's stack.  As
   * another example, if the stack is 32-bits wide and stackDepth is 400 then
   * 1600 bytes will be allocated for use as the task's stack.  The stack depth
   * multiplied by the stack width must not exceed the maximum value that can be
   * contained in a variable of type size_t.
   * @param name A descriptive name for the task. This is mainly used to
   * facilitate debugging, but can also be used to obtain a task handle.  The
   * maximum length of a task's name is defined by configMAX_TASK_NAME_LEN in
   * FreeRTOSConfig.h.
   *
   * <b>Example Usage</b>
   * @include Task/task.cpp
   */
  explicit Task(
      const UBaseType_t priority = tskIDLE_PRIORITY,
      const configSTACK_DEPTH_TYPE stackDepth = configMINIMAL_STACK_SIZE,
      const char* name = "") {
    taskCreatedSuccessfully = (xTaskCreate(callTaskFunction, name, stackDepth,
                                           this, priority, &handle) == pdPASS);
  }

  ~Task() = default;

  Task(Task&&) noexcept = default;
  Task& operator=(Task&&) noexcept = default;

 private:
  bool taskCreatedSuccessfully = false;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * StaticTask Task.hpp <FreeRTOS/Task.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS task.
 *
 * Each task requires RAM that is used to hold the task state, and used by the
 * task as its stack. If a task is created using this class then the RAM is
 * included in the object. If a task is created using FreeRTOS::Task() then the
 * required RAM is automatically allocated from the FreeRTOS heap.  See the
 * Static Vs Dynamic allocation page for more information.
 *
 * @note This class is not intended to be instantiated by the user.  The user
 * should create a class that derives from this class and implement
 * taskFunction().
 *
 * @warning This class contains the task's data structures (TCB) and the array
 * used to store the task's stack, so any instace of this class or class derived
 * from this class must be persistent (not declared on the stack of another
 * function).
 *
 * @tparam N The number of indexes in the array of <tt>StackType_t</tt> used to
 * store the stack for this task.
 */
template <UBaseType_t N = configMINIMAL_STACK_SIZE>
class StaticTask : public TaskBase {
 public:
  StaticTask(const StaticTask&) = delete;
  StaticTask& operator=(const StaticTask&) = delete;

 protected:
  /**
   * Task.hpp
   *
   * @brief Construct a new Task object by calling <tt>TaskHandle_t
   * xTaskCreateStatic( TaskFunction_t pxTaskCode, const char * const pcName,
   * const uint32_t ulStackDepth, void * const pvParameters, UBaseType_t
   * uxPriority, StackType_t * const puxStackBuffer, StaticTask_t * const
   * pxTaskBuffer )</tt>
   *
   * @see <https://www.freertos.org/xTaskCreateStatic.html>
   *
   * @note When calling <tt>xTaskCreateStatic</tt> the constructor passes the
   * <tt>this</tt> pointer as the task function argument. This pointer is used
   * so that the interface function callTaskFunction() can invoke taskEntry()
   * for this instance of the class.
   *
   * @param priority The priority at which the created task will execute.
   * Priorities are asserted to be less than configMAX_PRIORITIES. If
   * configASSERT is undefined, priorities are silently capped at
   * (configMAX_PRIORITIES - 1).
   * @param name A descriptive name for the task. This is mainly used to
   * facilitate debugging, but can also be used to obtain a task handle.  The
   * maximum length of a task's name is defined by configMAX_TASK_NAME_LEN in
   * FreeRTOSConfig.h.
   *
   * <b>Example Usage</b>
   * @include Task/staticTask.cpp
   */
  explicit StaticTask(const UBaseType_t priority = tskIDLE_PRIORITY,
                      const char* name = "") {
    handle = xTaskCreateStatic(callTaskFunction, name, N, this, priority, stack,
                               &taskBuffer);
  }
  ~StaticTask() = default;

  StaticTask(StaticTask&&) noexcept = default;
  StaticTask& operator=(StaticTask&&) noexcept = default;

 private:
  StaticTask_t taskBuffer;
  StackType_t stack[N];
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

inline void callTaskFunction(void* task) {
  static_cast<FreeRTOS::TaskBase*>(task)->taskEntry();
}

#endif  // FREERTOS_TASK_HPP
