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

#ifndef FREERTOS_MUTEX_HPP
#define FREERTOS_MUTEX_HPP

#include "FreeRTOS.h"
#include "semphr.h"

namespace FreeRTOS {

/**
 * @class MutexBase Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Base class that provides the standard mutex interface to
 * FreeRTOS::Mutex, FreeRTOS::StaticMutex, FreeRTOS::RecursiveMutex, and
 * FreeRTOS::StaticRecursiveMutex.
 *
 * @note This class is not intended to be instantiated by the user. Use
 * FreeRTOS::Mutex, FreeRTOS::StaticMutex, FreeRTOS::RecursiveMutex, and
 * FreeRTOS::StaticRecursiveMutex.
 */
class MutexBase {
 public:
  friend class Mutex;
  friend class StaticMutex;
  friend class RecursiveMutexBase;
  friend class RecursiveMutex;
  friend class StaticRecursiveMutex;

  MutexBase(const MutexBase&) = delete;
  MutexBase& operator=(const MutexBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * Mutex.hpp
   *
   * @brief Function that checks if the underlying semaphore handle is not NULL.
   * This should be used to ensure a semaphore has been created correctly.
   *
   * @retval true the handle is not NULL.
   * @retval false the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTake( SemaphoreHandle_t
   * xSemaphore, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/a00122.html>
   *
   * @param ticksToWait The time in ticks to wait for the mutex to become
   * available. The macro portTICK_PERIOD_MS can be used to convert this to a
   * real time. A block time of zero can be used to poll the mutex.
   * @retval true If the mutex was locked.
   * @retval false If ticksToWait expired without the mutex becoming available.
   *
   * <b>Example Usage</b>
   * @include Mutex/lock.cpp
   */
  inline bool lock(const TickType_t ticksToWait = portMAX_DELAY) const {
    return (xSemaphoreTake(handle, ticksToWait) == pdTRUE);
  }

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTakeFromISR ( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreTakeFromISR.html>
   *
   * @param higherPriorityTaskWoken It is possible (although unlikely, and
   * dependent on the semaphore type) that a mutex will have one or more tasks
   * blocked on it waiting to give the mutex. Calling lockFromISR() will make a
   * task that was blocked waiting to give the mutex leave the Blocked state. If
   * calling the API function causes a task to leave the Blocked state, and the
   * unblocked task has a priority equal to or higher than the currently
   * executing task (the task that was interrupted), then, internally, the API
   * function will set higherPriorityTaskWoken to true.
   * @return true If the mutex was successfully locked.
   * @return false If the mutex was not successfully locked because it was not
   * available.
   */
  inline bool lockFromISR(bool& higherPriorityTaskWoken) const {
    BaseType_t taskWoken = pdFALSE;
    bool result = (xSemaphoreTakeFromISR(handle, &taskWoken) == pdTRUE);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTakeFromISR ( SemaphoreHandle_t
   * xSemaphore, signed BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreTakeFromISR.html>
   *
   * @overload
   */
  inline bool lockFromISR() const {
    return (xSemaphoreTakeFromISR(handle, NULL) == pdTRUE);
  }

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreGive( SemaphoreHandle_t xSemaphore
   * )</tt>
   *
   * @see <https://www.freertos.org/a00123.html>
   *
   * @warning This must not be used from an ISR.
   *
   * @return true If the mutex was unlocked.
   * @return false If an error occurred. Mutexes (semaphores) are implemented
   * using queues. An error can occur if there is no space on the queue to post
   * a message indicating that the mutex was not first locked correctly.
   *
   * <b>Example Usage</b>
   * @include Mutex/unlock.cpp
   */
  inline bool unlock() const { return (xSemaphoreGive(handle) == pdTRUE); }

 private:
  MutexBase() = default;

  /**
   * Mutex.hpp
   *
   * @brief Destroy the MutexBase object by calling <tt>void vSemaphoreDelete(
   * SemaphoreHandle_t xSemaphore )</tt>
   *
   * @see <https://www.freertos.org/a00113.html#vSemaphoreDelete>
   *
   * @note Do not delete a mutex that has tasks blocked on it (tasks that are in
   * the Blocked state waiting for the mutex to become available).
   */
  ~MutexBase() { vSemaphoreDelete(this->handle); }

  MutexBase(MutexBase&&) noexcept = default;
  MutexBase& operator=(MutexBase&&) noexcept = default;

  /**
   * @brief Handle used to refer to the semaphore when using the FreeRTOS
   * interface.
   */
  SemaphoreHandle_t handle = NULL;
};

/**
 * @class RecursiveMutexBase Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Base class that provides the recursive mutex interface to
 * FreeRTOS::RecursiveMutex and FreeRTOS::StaticRecursiveMutex. This class
 * exists to override the lock() and unlock() functions which require different
 * underlying functions from what is used in FreeRTOS::MutexBase.
 *
 * @note This class is not intended to be instantiated by the user. Use
 * FreeRTOS::RecursiveMutex or FreeRTOS::StaticRecursiveMutex.
 */
class RecursiveMutexBase : public MutexBase {
 public:
  friend class RecursiveMutex;
  friend class StaticRecursiveMutex;

  RecursiveMutexBase(const RecursiveMutexBase&) = delete;
  RecursiveMutexBase& operator=(const RecursiveMutexBase&) = delete;

  static void* operator new(size_t, void*);
  static void* operator new[](size_t, void*);
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreTakeRecursive( SemaphoreHandle_t
   * xMutex, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreTakeRecursive.html>
   *
   * @param ticksToWait The time in ticks to wait for the mutex to become
   * available. The macro portTICK_PERIOD_MS can be used to convert this to a
   * real time. A block time of zero can be used to poll the mutex.  If the task
   * already owns the mutex then take() will return immediately no matter what
   * the value of ticksToWait.
   * @retval true If the mutex was locked.
   * @retval false If ticksToWait expired without the mutex becoming available.
   *
   * <b>Example Usage</b>
   * @include Mutex/recursiveLock.cpp
   */
  inline bool lock(const TickType_t ticksToWait = portMAX_DELAY) const {
    return (xSemaphoreTakeRecursive(handle, ticksToWait) == pdTRUE);
  }

  /**
   * Mutex.hpp
   *
   * @brief Function that calls <tt>xSemaphoreGiveRecursive( SemaphoreHandle_t
   * xSemaphore )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreGiveRecursive.html>
   *
   * A mutex used recursively can be locked repeatedly by the owner.  The mutex
   * doesn't become available again until the owner has called unlock() for each
   * successful lock request.  For example, if a task successfully locks the
   * same mutex 5 times then the mutex will not be available to any other task
   * until it has also unlocked the mutex back exactly five times.
   *
   * @return true If the mutex was unlocked.
   * @return false Otherwise.
   *
   * <b>Example Usage</b>
   * @include Mutex/recursiveLock.cpp
   */
  inline bool unlock() const {
    return (xSemaphoreGiveRecursive(handle) == pdTRUE);
  }

 private:
  RecursiveMutexBase() = default;
  ~RecursiveMutexBase() = default;

  RecursiveMutexBase(RecursiveMutexBase&&) noexcept = default;
  RecursiveMutexBase& operator=(RecursiveMutexBase&&) noexcept = default;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class Mutex Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS mutex.
 *
 * Each mutex require a small amount of RAM that is used to hold the mutex's
 * state. If a mutex is created using FreeRTOS::Mutex then the required RAM is
 * automatically allocated from the FreeRTOS heap. If a mutex is created using
 * FreeRTOS::StaticMutex then the RAM is provided by the application writer and
 * allows the RAM to be statically allocated at compile time. See the Static Vs
 * Dynamic allocation page for more information.
 *
 * Mutexes and binary semaphores are very similar but have some subtle
 * differences: Mutexes include a priority inheritance mechanism, binary
 * semaphores do not. This makes binary semaphores the better choice for
 * implementing synchronisation (between tasks or between tasks and an
 * interrupt), and mutexes the better choice for implementing simple mutual
 * exclusion.
 *
 * The priority of a task that locks a mutex will be temporarily raised if
 * another task of higher priority attempts to obtain the same mutex. The task
 * that owns the mutex 'inherits' the priority of the task attempting to lock
 * the same mutex. This means the mutex must always be unlocked back otherwise
 * the higher priority task will never be able to lock the mutex, and the lower
 * priority task will never 'disinherit' the priority.
 */
class Mutex : public MutexBase {
 public:
  /**
   * Mutex.hpp
   *
   * @brief Construct a new Mutex object by calling <tt>SemaphoreHandle_t
   * xSemaphoreCreateMutex( void )</tt>
   *
   * @see <https://www.freertos.org/CreateMutex.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * mutex was created successfully in case the memory required to create the
   * queue could not be allocated.
   *
   * <b>Example Usage</b>
   * @include Mutex/mutex.cpp
   */
  Mutex() { this->handle = xSemaphoreCreateMutex(); }
  ~Mutex() = default;

  Mutex(const Mutex&) = delete;
  Mutex& operator=(const Mutex&) = delete;

  Mutex(Mutex&&) noexcept = default;
  Mutex& operator=(Mutex&&) noexcept = default;
};

/**
 * @class RecursiveMutex Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS recursive
 * mutex.
 *
 * Each recursive mutex require a small amount of RAM that is used to hold the
 * recursive mutex's state. If a mutex is created using FreeRTOS::RecursiveMutex
 * then the required RAM is automatically allocated from the FreeRTOS heap. If a
 * recursive mutex is created using FreeRTOS::StaticRecursiveMutex then the RAM
 * is provided by the application writer, which requires an additional
 * parameter, but allows the RAM to be statically allocated at compile time. See
 * the Static Vs Dynamic allocation page for more information.
 *
 * Contrary to non-recursive mutexes, a task can lock a recursive mutex multiple
 * times, and the recursive mutex will only be returned after the holding task
 * has unlocked the mutex the same number of times it locked the mutex.
 *
 * Like non-recursive mutexes, recursive mutexes implement a priority
 * inheritance algorithm. The priority of a task that locks a mutex will be
 * temporarily raised if another task of higher priority attempts to obtain the
 * same mutex. The task that owns the mutex 'inherits' the priority of the task
 * attempting to lock the same mutex. This means the mutex must always be
 * unlocked otherwise the higher priority task will never be able to obtain the
 * mutex, and the lower priority task will never 'disinherit' the priority.
 */
class RecursiveMutex : public RecursiveMutexBase {
 public:
  /**
   * Mutex.hpp
   *
   * @brief Construct a new RecursiveMutex object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateRecursiveMutex( void )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateRecursiveMutex.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * recursive mutex was created successfully in case the memory required to
   * create the queue could not be allocated.
   *
   * <b>Example Usage</b>
   * @include Mutex/recursiveMutex.cpp
   */
  RecursiveMutex() { this->handle = xSemaphoreCreateRecursiveMutex(); }
  ~RecursiveMutex() = default;

  RecursiveMutex(const RecursiveMutex&) = delete;
  RecursiveMutex& operator=(const RecursiveMutex&) = delete;

  RecursiveMutex(RecursiveMutex&&) noexcept = default;
  RecursiveMutex& operator=(RecursiveMutex&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticMutex Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS mutex.
 *
 * Each mutex require a small amount of RAM that is used to hold the mutex's
 * state. If a mutex is created using FreeRTOS::Mutex then the required RAM is
 * automatically allocated from the FreeRTOS heap. If a mutex is created using
 * FreeRTOS::StaticMutex then the RAM is provided by the application writer and
 * allows the RAM to be statically allocated at compile time. See the Static Vs
 * Dynamic allocation page for more information.
 *
 * Mutexes and binary semaphores are very similar but have some subtle
 * differences: Mutexes include a priority inheritance mechanism, binary
 * semaphores do not. This makes binary semaphores the better choice for
 * implementing synchronisation (between tasks or between tasks and an
 * interrupt), and mutexes the better choice for implementing simple mutual
 * exclusion.
 *
 * The priority of a task that locks a mutex will be temporarily raised if
 * another task of higher priority attempts to obtain the same mutex. The task
 * that owns the mutex 'inherits' the priority of the task attempting to lock
 * the same mutex. This means the mutex must always be unlocked back otherwise
 * the higher priority task will never be able to lock the mutex, and the lower
 * priority task will never 'disinherit' the priority.
 */
class StaticMutex : public MutexBase {
 public:
  /**
   * Mutex.hpp
   *
   * @brief Construct a new StaticMutex object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t
   * *pxMutexBuffer )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateMutexStatic.html>
   *
   * @warning This class contains the storage buffer for the mutex, so the user
   * should create this object as a global object or with the static storage
   * specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include Mutex/staticMutex.cpp
   */
  StaticMutex() { this->handle = xSemaphoreCreateMutexStatic(&staticMutex); }
  ~StaticMutex() = default;

  StaticMutex(const StaticMutex&) = delete;
  StaticMutex& operator=(const StaticMutex&) = delete;

  StaticMutex(StaticMutex&&) noexcept = default;
  StaticMutex& operator=(StaticMutex&&) noexcept = default;

 private:
  StaticSemaphore_t staticMutex;
};

/**
 * @class StaticRecursiveMutex Mutex.hpp <FreeRTOS/Mutex.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS recursive
 * mutex.
 *
 * Each recursive mutex require a small amount of RAM that is used to hold the
 * recursive mutex's state. If a mutex is created using FreeRTOS::RecursiveMutex
 * then the required RAM is automatically allocated from the FreeRTOS heap. If a
 * recursive mutex is created using FreeRTOS::StaticRecursiveMutex then the RAM
 * is provided by the application writer, which requires an additional
 * parameter, but allows the RAM to be statically allocated at compile time. See
 * the Static Vs Dynamic allocation page for more information.
 *
 * Contrary to non-recursive mutexes, a task can lock a recursive mutex multiple
 * times, and the recursive mutex will only be returned after the holding task
 * has unlocked the mutex the same number of times it locked the mutex.
 *
 * Like non-recursive mutexes, recursive mutexes implement a priority
 * inheritance algorithm. The priority of a task that locks a mutex will be
 * temporarily raised if another task of higher priority attempts to obtain the
 * same mutex. The task that owns the mutex 'inherits' the priority of the task
 * attempting to lock the same mutex. This means the mutex must always be
 * unlocked otherwise the higher priority task will never be able to obtain the
 * mutex, and the lower priority task will never 'disinherit' the priority.
 */
class StaticRecursiveMutex : public RecursiveMutexBase {
 public:
  /**
   * Mutex.hpp
   *
   * @brief Construct a new StaticRecursiveMutex object by calling
   * <tt>SemaphoreHandle_t xSemaphoreCreateRecursiveMutexStatic(
   * StaticSemaphore_t *pxMutexBuffer )</tt>
   *
   * @see <https://www.freertos.org/xSemaphoreCreateRecursiveMutexStatic.html>
   *
   * @warning This class contains the storage buffer for the recursive mutex, so
   * the user should create this object as a global object or with the static
   * storage specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include Mutex/staticRecursiveMutex.cpp
   */
  StaticRecursiveMutex() {
    this->handle = xSemaphoreCreateRecursiveMutexStatic(&staticRecursiveMutex);
  }
  ~StaticRecursiveMutex() = default;

  StaticRecursiveMutex(const StaticRecursiveMutex&) = delete;
  StaticRecursiveMutex& operator=(const StaticRecursiveMutex&) = delete;

  StaticRecursiveMutex(StaticRecursiveMutex&&) noexcept = default;
  StaticRecursiveMutex& operator=(StaticRecursiveMutex&&) noexcept = default;

 private:
  StaticSemaphore_t staticRecursiveMutex;
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_MUTEX_HPP
