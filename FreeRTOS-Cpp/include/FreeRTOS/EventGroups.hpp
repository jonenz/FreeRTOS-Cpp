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

#ifndef FREERTOS_EVENTGROUPS_HPP
#define FREERTOS_EVENTGROUPS_HPP

#include <bitset>

#include "FreeRTOS.h"
#include "event_groups.h"

namespace FreeRTOS {

/**
 * @class EventGroupBase EventGroupBase.hpp <FreeRTOS/EventGroups.hpp>
 *
 * @brief Base class that provides the standard event group interface to
 * FreeRTOS::EventGroup and FreeRTOS::StaticEventGroup.
 *
 * @note This class is not intended to be instantiated by the user.  Use
 * FreeRTOS::EventGroup or FreeRTOS::StaticEventGroup.
 */
class EventGroupBase {
 public:
  friend class EventGroup;
  friend class StaticEventGroup;

  EventGroupBase(const EventGroupBase&) = delete;
  EventGroupBase& operator=(const EventGroupBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  // NOLINTNEXTLINE
  using EventBits = std::bitset<((configUSE_16_BIT_TICKS == 1) ? 8 : 24)>;

  /**
   * EventGroups.hpp
   *
   * @brief Function that checks if the underlying event group handle is not
   * NULL.  This should be used to ensure an event group has been created
   * correctly.
   *
   * @retval true the handle is not NULL.
   * @retval false the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupWaitBits( const
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const
   * BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t
   * xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupWaitBits.html>
   *
   * Read bits within an RTOS event group, optionally entering the Blocked state
   * (with a timeout) to wait for a bit or group of bits to become set.
   *
   * @warning This function cannot be called from an interrupt.
   *
   * @param bitsToWaitFor A bitwise value that indicates the bit or bits to test
   * inside the event group.  bitsToWaitFor must not be set to 0.
   * @param clearOnExit If clearOnExit is set to true then any bits set in the
   * value passed as the bitsToWaitFor parameter will be cleared in the event
   * group before wait() returns if wait() returns for any reason other than a
   * timeout. The timeout value is set by the ticksToWait parameter. If
   * clearOnExit is set to false then the bits set in the event group are not
   * altered when the call to wait() returns.
   * @param waitForAllBits waitForAllBits is used to create either a logical AND
   * test (where all bits must be set) or a logical OR test (where one or more
   * bits must be set) as follows: If waitForAllBits is set to true then wait()
   * will return when either all the bits set in the value passed as the
   * bitsToWaitFor parameter are set in the event group or the specified block
   * time expires. If waitForAllBits is set to false then wait() will return
   * when any of the bits set in the value passed as the bitsToWaitFor parameter
   * are set in the event group or the specified block time expires.
   * @param ticksToWait	The maximum amount of time (specified in 'ticks') to
   * wait for one/all (depending on the waitForAllBits value) of the bits
   * specified by bitsToWaitFor to become set.
   * @return EventBits The value of the event group at the time either the event
   * bits being waited for became set, or the block time expired. The current
   * value of the event bits in an event group will be different to the returned
   * value if a higher priority task or interrupt changed the value of an event
   * bit between the calling task leaving the Blocked state and exiting the
   * wait() function. Test the return value to know which bits were set. If
   * wait() returned because its timeout expired then not all the bits being
   * waited for will be set. If wait() returned because the bits it was waiting
   * for were set then the returned value is the event group value before any
   * bits were automatically cleared because the clearOnExit parameter was set
   * to true.
   *
   * <b>Example Usage</b>
   * @include EventGroups/wait.cpp
   */
  inline EventBits wait(const EventBits& bitsToWaitFor = 0,
                        const bool clearOnExit = false,
                        const bool waitForAllBits = false,
                        const TickType_t ticksToWait = portMAX_DELAY) const {
    return EventBits(xEventGroupWaitBits(
        handle, bitsToWaitFor.to_ulong(), (clearOnExit ? pdTRUE : pdFALSE),
        (waitForAllBits ? pdTRUE : pdFALSE), ticksToWait));
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupSetBits(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupSetBits.html>
   *
   * Set bits (flags) within an RTOS event group. This function cannot be called
   * from an interrupt. setFromISR() is a version that can be called from an
   * interrupt.
   *
   * Setting bits in an event group will automatically unblock tasks that are
   * blocked waiting for the bits.
   *
   * @param bitsToSet A bitwise value that indicates the bit or bits to set in
   * the event group.
   * @return EventBits The value of the event group at the time the call to
   * set() returns. There are two reasons why the returned value might have the
   * bits specified by the uxBitsToSet parameter cleared:
   *  1. If setting a bit results in a task that was waiting for the bit leaving
   * the blocked state then it is possible the bit will have been cleared
   * automatically (see the clearOnExit parameter of wait()).
   *  2. Any unblocked (or otherwise Ready state) task that has a priority above
   * that of the task that called set() will execute and may change the event
   * group value before the call to set() returns.
   *
   * <b>Example Usage</b>
   * @include EventGroups/set.cpp
   */
  inline EventBits set(const EventBits& bitsToSet) const {
    return EventBits(xEventGroupSetBits(handle, bitsToSet.to_ulong()));
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>BaseType_t xEventGroupSetBitsFromISR(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupSetBitsFromISR.html>
   *
   * Set bits (flags) within an RTOS event group. A version of set() that can be
   * called from an interrupt service routine (ISR).
   *
   * Setting bits in an event group will automatically unblock tasks that are
   * blocked waiting for the bits.
   *
   * Setting bits in an event group is not a deterministic operation because
   * there are an unknown number of tasks that may be waiting for the bit or
   * bits being set. FreeRTOS does not allow non-deterministic operations to be
   * performed in interrupts or from critical sections. Therefore
   * xEventGroupSetBitFromISR() sends a message to the RTOS daemon task to have
   * the set operation performed in the context of the daemon task - where a
   * scheduler lock is used in place of a critical section.
   *
   * @note As mentioned in the paragraph above, setting bits from an ISR will
   * defer the set operation to the RTOS daemon task (also known as the timer
   * service task). The RTOS daemon task is scheduled according to its priority,
   * just like any other RTOS task. Therefore, if it is essential the set
   * operation completes immediately (before a task created by the application
   * executes) then the priority of the RTOS daemon task must be higher than the
   * priority of any application task that uses the event group. The priority of
   * the RTOS daemon task is set by the configTIMER_TASK_PRIORITY definition in
   * FreeRTOSConfig.h.
   *
   * @param higherPriorityTaskWoken As mentioned above, calling this function
   * will result in a message being sent to the RTOS daemon task. If the
   * priority of the daemon task is higher than the priority of the currently
   * running task (the task the interrupt interrupted) then
   * higherPriorityTaskWoken will be set to true by setFromISR(), indicating
   * that a context switch should be requested before the interrupt exits. For
   * that reason higherPriorityTaskWoken must be initialised to false. See the
   * example code below.
   * @param bitsToSet A bitwise value that indicates the bit or bits to set in
   * the event group.
   * @retval true If the message was sent to the RTOS daemon task.
   * @retval false Otherwise or if the timer service queue was full
   *
   * <b>Example Usage</b>
   * @include EventGroups/setFromISR.cpp
   */
  inline bool setFromISR(bool& higherPriorityTaskWoken,
                         const EventBits& bitsToSet) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xEventGroupSetBitsFromISR(handle, bitsToSet.to_ulong(),
                                             &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>BaseType_t xEventGroupSetBitsFromISR(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupSetBitsFromISR.html>
   *
   * @overload
   */
  inline bool setFromISR(const EventBits& bitsToSet) const {
    return (xEventGroupSetBitsFromISR(handle, bitsToSet.to_ulong(), NULL) ==
            pdPASS);
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupClearBits(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupClearBits.html>
   *
   * Clear bits (flags) within an RTOS event group. This function cannot be
   * called from an interrupt. See clearFromISR() for a version that can be
   * called from an interrupt.
   *
   * @param bitsToClear A bitwise value that indicates the bit or bits to clear
   * in the event group.
   * @return EventBits The value of the event group before the specified bits
   * were cleared.
   *
   * <b>Example Usage</b>
   * @include EventGroups/clear.cpp
   */
  inline EventBits clear(const EventBits& bitsToClear) const {
    return EventBits(xEventGroupClearBits(handle, bitsToClear.to_ulong()));
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>BaseType_t xEventGroupClearBitsFromISR(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupClearBitsFromISR.html>
   *
   * A version of clear() that can be called from an interrupt. The clear
   * operation is deferred to the RTOS daemon task which is also known as the
   * timer service task. The priority of the daemon task is set by the
   * configTIMER_TASK_PRIORITY setting in FreeRTOSConfig.h.
   *
   * @param bitsToClear A bitwise value that indicates the bit or bits to clear
   * in the event group.
   * @return true If the operation was successfully deferred to the RTOS daemon
   * task.
   * @return false If the timer command queue is full.
   *
   * <b>Example Usage</b>
   * @include EventGroups/clearFromISR.cpp
   */
  inline bool clearFromISR(const EventBits& bitsToClear) const {
    return (xEventGroupClearBitsFromISR(handle, bitsToClear.to_ulong()) ==
            pdPASS);
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupGetBits(
   * EventGroupHandle_t xEventGroup )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupGetBits.html>
   *
   * Returns the current value of the event bits (event flags) in an RTOS event
   * group. This function cannot be used from an interrupt. See getFromISR() for
   * a version that can be used in an interrupt.
   *
   * @return EventBits The value of the event bits in the event group at the
   * time get() was called.
   */
  inline EventBits get() const { return EventBits(xEventGroupGetBits(handle)); }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupGetBitsFromISR(
   * EventGroupHandle_t xEventGroup )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupGetBitsFromISR.html>
   *
   * A version of get() that can be called from an interrupt.
   *
   * @return EventBits The value of the event bits in the event group at the
   * time getFromISR() was called.
   */
  inline EventBits getFromISR() const {
    return EventBits(xEventGroupGetBitsFromISR(handle));
  }

  /**
   * EventGroups.hpp
   *
   * @brief Function that calls <tt>EventBits_t xEventGroupSync(
   * EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const
   * EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupSync.html>
   *
   * Atomically set bits (flags) within an RTOS event group, then wait for a
   * combination of bits to be set within the same event group. This
   * functionality is typically used to synchronize multiple tasks (often called
   * a task rendezvous), where each task has to wait for the other tasks to
   * reach a synchronization point before proceeding.
   *
   * This function cannot be used from an interrupt.
   *
   * The function will return before its block time expires if the bits
   * specified by the bitsToWait parameter are set, or become set within that
   * time. In this case all the bits specified by bitsToWait will be
   * automatically cleared before the function returns.
   *
   * @param bitsToSet The bit or bits to set in the event group before
   * determining if (and possibly waiting for), all the bits specified by the
   * bitsToWait parameter are set.
   * @param bitsToWaitFor A bitwise value that indicates the bit or bits to test
   * inside the event group.
   * @param ticksToWait The maximum amount of time (specified in 'ticks') to
   * wait for all the bits specified by the uxBitsToWaitFor parameter value to
   * become set.
   * @return EventBits
   *
   * <b>Example Usage</b>
   * @include EventGroups/sync.cpp
   */
  inline EventBits sync(const EventBits& bitsToSet = 0,
                        const EventBits& bitsToWaitFor = 0,
                        const TickType_t ticksToWait = portMAX_DELAY) const {
    return EventBits(xEventGroupSync(handle, bitsToSet.to_ulong(),
                                     bitsToWaitFor.to_ulong(), ticksToWait));
  }

 private:
  /**
   * EventGroups.hpp
   *
   * @brief Construct a new EventGroupBase object.
   *
   * @note Default constructor is deliberately private as this class is not
   * intended to be instantiated or derived from by the user. Use
   * FreeRTOS::EventGroup or FreeRTOS::StaticEventGroup.
   */
  EventGroupBase() = default;

  /**
   * EventGroup.hpp
   *
   * @brief Destroy the EventGroupBase object by calling <tt>void
   * vEventGroupDelete( EventGroupHandle_t xEventGroup )</tt>
   *
   * @see <https://www.freertos.org/vEventGroupDelete.html>
   *
   * Delete an event group.
   *
   * Tasks that are blocked on the event group being deleted will be unblocked,
   * and report an event group value of 0.
   */
  ~EventGroupBase() { vEventGroupDelete(this->handle); };

  EventGroupBase(EventGroupBase&&) noexcept = default;
  EventGroupBase& operator=(EventGroupBase&&) noexcept = default;

  /**
   * @brief Handle used to refer to the event group when using the FreeRTOS
   * interface.
   */
  EventGroupHandle_t handle = NULL;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class EventGroup EventGroups.hpp <FreeRTOS/EventGroups.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS event group.
 *
 * Each event group requires a [very] small amount of RAM that is used to hold
 * the event group's state. If an event group is created using this class then
 * the required RAM is automatically allocated from the FreeRTOS heap. If an
 * event group is created using FreeRTOS::StaticEventGroup then the RAM is
 * provided by the application writer, which requires an additional parameter,
 * but allows the RAM to be statically allocated at compile time. See the Static
 * Vs Dynamic allocation page for more information.
 */
class EventGroup : public EventGroupBase {
 public:
  /**
   * EventGroup.hpp
   *
   * @brief Construct a new EventGroup object by calling <tt>EventGroupHandle_t
   * xEventGroupCreate( void )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupCreate.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * queue was created successfully in case the memory required to create the
   * queue could not be allocated.
   *
   * <b>Example Usage</b>
   * @include EventGroups/eventGroup.cpp
   */
  EventGroup() { this->handle = xEventGroupCreate(); }
  ~EventGroup() = default;

  EventGroup(const EventGroup&) = delete;
  EventGroup& operator=(const EventGroup&) = delete;

  EventGroup(EventGroup&&) noexcept = default;
  EventGroup& operator=(EventGroup&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticEventGroup EventGroups.hpp <FreeRTOS/EventGroups.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS event group.
 *
 * Each event group requires a [very] small amount of RAM that is used to hold
 * the event group's state. If an event group is created using
 * FreeRTOS::EventGroup then the required RAM is automatically allocated from
 * the FreeRTOS heap. If an event group is created using this class then the RAM
 * is provided by the application writer, which requires an additional
 * parameter, but allows the RAM to be statically allocated at compile time. See
 * the Static Vs Dynamic allocation page for more information.
 */
class StaticEventGroup : public EventGroupBase {
 public:
  /**
   * EventGroups.hpp
   *
   * @brief Construct a new StaticEventGroup object by calling
   * <tt>EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t
   * *pxEventGroupBuffer )</tt>
   *
   * @see <https://www.freertos.org/xEventGroupCreateStatic.html>
   *
   * @warning This class contains the storage buffer for the event group, so the
   * user should create this object as a global object or with the static
   * storage specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include EventGroups/staticEventGroup.cpp
   */
  StaticEventGroup() {
    this->handle = xEventGroupCreateStatic(&staticEventGroup);
  }
  ~StaticEventGroup() = default;

  StaticEventGroup(const StaticEventGroup&) = delete;
  StaticEventGroup& operator=(const StaticEventGroup&) = delete;

  StaticEventGroup(StaticEventGroup&&) noexcept = default;
  StaticEventGroup& operator=(StaticEventGroup&&) noexcept = default;

 private:
  StaticEventGroup_t staticEventGroup;
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_EVENTGROUPS_HPP
