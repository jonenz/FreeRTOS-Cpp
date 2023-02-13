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

#ifndef FREERTOS_SEMAPHORE_HPP
#define FREERTOS_SEMAPHORE_HPP

#include "FreeRTOS.h"
#include "semphr.h"

namespace FreeRTOS {

/**
 * @class SemaphoreBase Semaphore.hpp <FreeRTOS/Semaphore.hpp>
 *
 * @brief Base class that provides the standard semaphore interface to
 * FreeRTOS::BinarySemaphore, FreeRTOS::StaticBinarySemaphore,
 * FreeRTOS::CountingSemaphore, and FreeRTOS::StaticCountingSemaphore.
 *
 * @note This class is not intended to be instantiated by the user. Use
 * FreeRTOS::BinarySemaphore, FreeRTOS::StaticBinarySemaphore,
 * FreeRTOS::CountingSemaphore, or FreeRTOS::StaticCountingSemaphore.
 */
class SemaphoreBase {
 public:
  friend class BinarySemaphore;
  friend class StaticBinarySemaphore;
  friend class CountingSemaphore;
  friend class StaticCountingSemaphore;

  SemaphoreBase(const SemaphoreBase&) = delete;
  SemaphoreBase& operator=(const SemaphoreBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * Semaphore.hpp
   *
   * @brief Function that checks if the underlying semaphore handle is not NULL.
   * This should be used to ensure a semaphore has been created correctly.
   *
   * @retval true the handle is not NULL.
   * @retval false the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxSemaphoreGetCount(
   * SemaphoreHandle_t xSemaphore )</tt>
   *
   * @see <https://www.freertos.org/uxSemaphoreGetCount.html>
   *
   * Returns the count of a semaphore.
   *
   * @return UBaseType_t If the semaphore is a counting semaphore then the
   * semaphores current count value is returned. If the semaphore is a binary
   * semaphore then 1 is returned if the semaphore is available, and 0 is
   * returned if the semaphore is not available.
   */
  inline UBaseType_t getCount() const { return uxSemaphoreGetCount(handle); }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTake( SemaphoreHandle_t
   * xSemaphore, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/a00122.html>
   *
   * Function to obtain a semaphore.
   *
   * This macro must not be called from an ISR. takeFromISR() can be used to
   * take a semaphore from within an interrupt if required, although this would
   * not be a normal operation. Semaphores use queues as their underlying
   * mechanism, so functions are to some extent interoperable.
   *
   * @param ticksToWait The time in ticks to wait for the semaphore to become
   * available. The macro portTICK_PERIOD_MS can be used to convert this to a
   * real time. A block time of zero can be used to poll the semaphore.
   * @retval true If the semaphore was obtained.
   * @retval false If xTicksToWait expired without the semaphore becoming
   * available.
   *
   * <b>Example Usage</b>
   * @include Semaphore/take.cpp
   */
  inline bool take(const TickType_t ticksToWait = portMAX_DELAY) const {
    return (xSemaphoreTake(handle, ticksToWait) == pdTRUE);
  }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTakeFromISR( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken)</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreTakeFromISR.html>
   *
   * A version of take() that can be called from an ISR. Unlike take(),
   * takeFromISR() does not permit a block time to be specified.
   *
   * @param higherPriorityTaskWoken It is possible (although unlikely, and
   * dependent on the semaphore type) that a semaphore will have one or more
   * tasks blocked on it waiting to give the semaphore. Calling takeFromISR()
   * will make a task that was blocked waiting to give the semaphore leave the
   * Blocked state. If calling the API function causes a task to leave the
   * Blocked state, and the unblocked task has a priority equal to or higher
   * than the currently executing task (the task that was interrupted), then,
   * internally, the API function will set higherPriorityTaskWoken to true. If
   * takeFromISR() sets higherPriorityTaskWoken to true, then a context switch
   * should be performed before the interrupt is exited. This will ensure that
   * the interrupt returns directly to the highest priority Ready state task.
   * The mechanism is identical to that used in the FreeRTOS::receiveFromISR()
   * function, and readers are referred to the FreeRTOS::receiveFromISR()
   * documentation for further explanation.
   * @retval true If the semaphore was successfully taken.
   * @retval false If the semaphore was not successfully taken because it was
   * not available.
   */
  inline bool takeFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xSemaphoreTakeFromISR(handle, &taskWoken) == pdTRUE);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTakeFromISR( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken)</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreTakeFromISR.html>
   *
   * @overload
   */
  inline bool takeFromISR() const {
    return (xSemaphoreTakeFromISR(handle, NULL) == pdTRUE);
  }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreGive( SemaphoreHandle_t xSemaphore
   * )</tt>
   *
   * @see <https://www.freertos.org/a00123.html>
   *
   * Function to release a semaphore.
   *
   * This must not be used from an ISR. See giveFromISR() for an alternative
   * which can be used from an ISR.
   *
   * @retval true If the semaphore was released.
   * @retval false If an error occurred. Semaphores are implemented using
   * queues. An error can occur if there is no space on the queue to post a
   * message indicating that the semaphore was not first obtained correctly.
   *
   * <b>Example Usage</b>
   * @include Semaphore/give.cpp
   */
  inline bool give() const { return (xSemaphoreGive(handle) == pdTRUE); }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreGiveFromISR( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/a00124.html>
   *
   * Function to release a semaphore.
   *
   * This macro can be used from an ISR.
   *
   * @param higherPriorityTaskWoken giveFromISR() will set
   * higherPriorityTaskWoken to true if giving the semaphore caused a task to
   * unblock, and the unblocked task has a priority higher than the currently
   * running task. If giveFromISR() sets this value to true then a context
   * switch should be requested before the interrupt is exited.
   * @retval true If the semaphore was successfully given.
   * @retval false Otherwise.
   *
   * <b>Example Usage</b>
   * @include Semaphore/giveFromISR.cpp
   */
  inline bool giveFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xSemaphoreGiveFromISR(handle, &taskWoken) == pdTRUE);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Semaphore.hpp
   *
   * @brief Function that calls <tt>xSemaphoreGiveFromISR( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/a00124.html>
   *
   * @overload
   */
  inline bool giveFromISR() const {
    return (xSemaphoreGiveFromISR(handle, NULL) == pdTRUE);
  }

 private:
  SemaphoreBase() = default;

  /**
   * Semaphore.hpp
   *
   * @brief Destroy the SemaphoreBase object by calling <tt>void
   * vSemaphoreDelete( SemaphoreHandle_t xSemaphore )</tt>
   *
   * @see <https://www.freertos.org/a00113.html#vSemaphoreDelete>
   *
   * @note Do not delete a semaphore that has tasks blocked on it (tasks that
   * are in the Blocked state waiting for the semaphore to become available).
   */
  ~SemaphoreBase() { vSemaphoreDelete(this->handle); }

  SemaphoreBase(SemaphoreBase&&) noexcept = default;
  SemaphoreBase& operator=(SemaphoreBase&&) noexcept = default;

  /**
   * @brief Handle used to refer to the semaphore when using the FreeRTOS
   * interface.
   */
  SemaphoreHandle_t handle = NULL;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class BinarySemaphore Semaphore.hpp <FreeRTOS/Semaphore.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS binary
 * semaphore.
 *
 * Each binary semaphore require a small amount of RAM that is used to hold the
 * semaphore's state. If a binary semaphore is created using
 * FreeRTOS::BinarySemaphore then the required RAM is automatically allocated
 * from the FreeRTOS heap. If a binary semaphore is created using
 * FreeRTOS::StaticBinarySemaphore then the RAM is provided by the application
 * writer as part of the class and allows the RAM to be statically allocated at
 * compile time. See the Static Vs Dynamic allocation page for more information.
 *
 * The semaphore is created in the 'empty' state, meaning the semaphore must
 * first be given using the give() API function before it can subsequently be
 * taken (obtained) using the take() function.
 *
 * Binary semaphores and mutexes are very similar but have some subtle
 * differences: Mutexes include a priority inheritance mechanism, binary
 * semaphores do not. This makes binary semaphores the better choice for
 * implementing synchronisation (between tasks or between tasks and an
 * interrupt), and mutexes the better choice for implementing simple mutual
 * exclusion.
 *
 * A binary semaphore need not be given back once obtained, so task
 * synchronisation can be implemented by one task/interrupt continuously
 * 'giving' the semaphore while another continuously 'takes' the semaphore. This
 * is demonstrated by the sample code on the giveFromISR() documentation page.
 * Note the same functionality can often be achieved in a more efficient way
 * using a direct to task notification.
 *
 * The priority of a task that 'takes' a mutex can potentially be raised if
 * another task of higher priority attempts to obtain the same mutex. The task
 * that owns the mutex 'inherits' the priority of the task attempting to 'take'
 * the same mutex. This means the mutex must always be 'given' back - otherwise
 * the higher priority task will never be able to obtain the mutex, and the
 * lower priority task will never 'disinherit' the priority. An example of a
 * mutex being used to implement mutual exclusion is provided on the take()
 * documentation page.
 */
class BinarySemaphore : public SemaphoreBase {
 public:
  /**
   * Semaphore.hpp
   *
   * @brief Construct a new BinarySemaphore object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateBinary( void )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateBinary.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * binary semaphore was created successfully in case the memory required to
   * create the queue could not be allocated.
   *
   * <b>Example Usage</b>
   * @include Semaphore/binarySemaphore.cpp
   */
  BinarySemaphore() { this->handle = xSemaphoreCreateBinary(); }
  ~BinarySemaphore() = default;

  BinarySemaphore(const BinarySemaphore&) = delete;
  BinarySemaphore& operator=(const BinarySemaphore&) = delete;

  BinarySemaphore(BinarySemaphore&&) noexcept = default;
  BinarySemaphore& operator=(BinarySemaphore&&) noexcept = default;
};

/**
 * @class CountingSemaphore Semaphore.hpp <FreeRTOS/Semaphore.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS counting
 * semaphore.
 *
 * Each counting semaphore require a small amount of RAM that is used to hold
 * the semaphore's state. If a counting semaphore is created using
 * FreeRTOS::CountingSemaphore then the required RAM is automatically allocated
 * from the FreeRTOS heap. If a counting semaphore is created using
 * FreeRTOS::StaticCountingSemaphore then the RAM is provided by the application
 * writer as part of the class and allows the RAM to be statically allocated at
 * compile time. See the Static Vs Dynamic allocation page for more information.
 *
 * Counting semaphores are typically used for two things:
 *  1. <b>Counting Events:</b>
 * In this usage scenario an event handler will 'give' a semaphore each time an
 * event occurs (incrementing the semaphore count value), and a handler task
 * will 'take' a semaphore each time it processes an event (decrementing the
 * semaphore count value). The count value is therefore the difference between
 * the number of events that have occurred and the number that have been
 * processed. In this case it is desirable for the initial count value to be
 * zero. Note the same functionality can often be achieved in a more efficient
 * way using a direct to task notification.
 *
 *  2. <b>Resource Management:</b>
 * In this usage scenario the count value indicates the number of resources
 * available. To obtain control of a resource a task must first obtain a
 * semaphore - decrementing the semaphore count value. When the count value
 * reaches zero there are no free resources. When a task finishes with the
 * resource it 'gives' the semaphore back - incrementing the semaphore count
 * value. In this case it is desirable for the initial count value to be equal
 * to the maximum count value, indicating that all resources are free.
 */
class CountingSemaphore : public SemaphoreBase {
 public:
  /**
   * Semaphore.hpp
   *
   * @brief Construct a new CountingSemaphore by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateCounting( UBaseType_t uxMaxCount,
   * UBaseType_t uxInitialCount)</tt>
   *
   * @warning The user should call isValid() on this object to verify that the
   * binary semaphore was created successfully in case the memory required to
   * create the queue could not be allocated.
   *
   * @param maxCount The maximum count value that can be reached. When the
   * semaphore reaches this value it can no longer be 'given'.
   * @param initialCount 	The count value assigned to the semaphore when
   * it is created.
   */
  explicit CountingSemaphore(const UBaseType_t maxCount,
                             const UBaseType_t initialCount = 0) {
    this->handle = xSemaphoreCreateCounting(maxCount, initialCount);
  }
  ~CountingSemaphore() = default;

  CountingSemaphore(const CountingSemaphore&) = delete;
  CountingSemaphore& operator=(const CountingSemaphore&) = delete;

  CountingSemaphore(CountingSemaphore&&) noexcept = default;
  CountingSemaphore& operator=(CountingSemaphore&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticBinarySemaphore Semaphore.hpp <FreeRTOS/Semaphore.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS binary
 * semaphore.
 *
 * Each binary semaphore require a small amount of RAM that is used to hold the
 * semaphore's state. If a binary semaphore is created using
 * FreeRTOS::BinarySemaphore then the required RAM is automatically allocated
 * from the FreeRTOS heap. If a binary semaphore is created using
 * FreeRTOS::StaticBinarySemaphore then the RAM is provided by the application
 * writer as part of the class and allows the RAM to be statically allocated at
 * compile time. See the Static Vs Dynamic allocation page for more information.
 *
 * The semaphore is created in the 'empty' state, meaning the semaphore must
 * first be given using the give() API function before it can subsequently be
 * taken (obtained) using the take() function.
 *
 * Binary semaphores and mutexes are very similar but have some subtle
 * differences: Mutexes include a priority inheritance mechanism, binary
 * semaphores do not. This makes binary semaphores the better choice for
 * implementing synchronisation (between tasks or between tasks and an
 * interrupt), and mutexes the better choice for implementing simple mutual
 * exclusion.
 *
 * A binary semaphore need not be given back once obtained, so task
 * synchronisation can be implemented by one task/interrupt continuously
 * 'giving' the semaphore while another continuously 'takes' the semaphore. This
 * is demonstrated by the sample code on the giveFromISR() documentation page.
 * Note the same functionality can often be achieved in a more efficient way
 * using a direct to task notification.
 *
 * The priority of a task that 'takes' a mutex can potentially be raised if
 * another task of higher priority attempts to obtain the same mutex. The task
 * that owns the mutex 'inherits' the priority of the task attempting to 'take'
 * the same mutex. This means the mutex must always be 'given' back - otherwise
 * the higher priority task will never be able to obtain the mutex, and the
 * lower priority task will never 'disinherit' the priority. An example of a
 * mutex being used to implement mutual exclusion is provided on the take()
 * documentation page.
 */
class StaticBinarySemaphore : public SemaphoreBase {
 public:
  /**
   * Semaphore.hpp
   *
   * @brief Construct a new StaticBinarySemaphore object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateBinaryStatic( StaticSemaphore_t
   * *pxSemaphoreBuffer )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateBinaryStatic.html>
   *
   * @warning This class contains the storage buffer for the binary semaphore,
   * so the user should create this object as a global object or with the static
   * storage specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include Semaphore/staticBinarySemaphore.cpp
   */
  StaticBinarySemaphore() {
    this->handle = xSemaphoreCreateBinaryStatic(&staticBinarySemaphore);
  }
  ~StaticBinarySemaphore() = default;

  StaticBinarySemaphore(const StaticBinarySemaphore&) = delete;
  StaticBinarySemaphore& operator=(const StaticBinarySemaphore&) = delete;

  StaticBinarySemaphore(StaticBinarySemaphore&&) noexcept = default;
  StaticBinarySemaphore& operator=(StaticBinarySemaphore&&) noexcept = default;

 private:
  StaticSemaphore_t staticBinarySemaphore;
};

class StaticCountingSemaphore : public SemaphoreBase {
 public:
  /**
   * @brief Construct a new StaticCountingSemaphore object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateCountingStatic( UBaseType_t
   * uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t
   * *pxSemaphoreBuffer )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateCountingStatic.html>
   *
   * @warning This class contains the storage buffer for the counting semaphore,
   * so the user should create this object as a global object or with the static
   * storage specifier so that the object instance is not on the stack.
   *
   * @param maxCount The maximum count value that can be reached. When the
   * semaphore reaches this value it can no longer be 'given'.
   * @param initialCount The count value assigned to the semaphore when it is
   * created.
   *
   * <b>Example Usage</b>
   * @include Semaphore/staticCountingSemaphore.cpp
   */
  explicit StaticCountingSemaphore(const UBaseType_t maxCount,
                                   const UBaseType_t initialCount = 0) {
    this->handle = xSemaphoreCreateCountingStatic(maxCount, initialCount,
                                                  &staticCountingSemaphore);
  }
  ~StaticCountingSemaphore() = default;

  StaticCountingSemaphore(const StaticCountingSemaphore&) = delete;
  StaticCountingSemaphore& operator=(const StaticCountingSemaphore&) = delete;

  StaticCountingSemaphore(StaticCountingSemaphore&&) noexcept = default;
  StaticCountingSemaphore& operator=(StaticCountingSemaphore&&) noexcept =
      default;

 private:
  StaticSemaphore_t staticCountingSemaphore;
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_SEMAPHORE_HPP