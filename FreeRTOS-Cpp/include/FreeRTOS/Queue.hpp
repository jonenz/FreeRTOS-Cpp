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

#ifndef FREERTOS_QUEUE_HPP
#define FREERTOS_QUEUE_HPP

#include <optional>

#include "FreeRTOS.h"
#include "queue.h"

namespace FreeRTOS {

/**
 * @class QueueBase Queue.hpp <FreeRTOS/Queue.hpp>
 *
 * @brief Base class that provides the standard queue interface to
 * FreeRTOS::Queue and FreeRTOS::StaticQueue.
 *
 * @note This class is not intended to be instantiated by the user.  Use
 * FreeRTOS::Queue or FreeRTOS::StaticQueue.
 *
 * @tparam T Type to be stored in the queue.
 */
template <class T>
class QueueBase {
 public:
  template <class>
  friend class Queue;

  template <class, UBaseType_t>
  friend class StaticQueue;

  QueueBase(const QueueBase&) = delete;
  QueueBase& operator=(const QueueBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * Queue.hpp
   *
   * @brief Function that checks if the underlying queue handle is not NULL.
   * This should be used to ensure a queue has been created correctly.
   *
   * @retval true the handle is not NULL.
   * @retval false the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToBack( xQueue, pvItemToQueue,
   * xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/a00117.html>
   *
   * Post an item to the back of a queue.  The item is queued by copy, not by
   * reference.  This function must not be called from an interrupt service
   * routine.  See FreeRTOS::Queue::sendToBackFromISR() for an alternative which
   * may be used in an ISR.
   *
   * @param item A reference to the item that is to be placed on the queue.
   * @param ticksToWait The maximum amount of time the task should block waiting
   * for space to become available on the queue, should it already be full.  The
   * call will return immediately if this is set to 0 and the queue is full. The
   * time is defined in tick periods so the constant portTICK_PERIOD_MS should
   * be used to convert to real time if this is required.
   * @retval true if the item was successfully posted.
   * @retval false otherwise.
   *
   * <b>Example Usage</b>
   * @include Queue/sendToBack.cpp
   */
  inline bool sendToBack(const T& item,
                         const TickType_t ticksToWait = portMAX_DELAY) const {
    return (xQueueSendToBack(handle, &item, ticksToWait) == pdTRUE);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToBackFromISR( xQueue,
   * pvItemToQueue, pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueSendToBackFromISR.html>
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending to the queue caused a task to unblock, and the unblocked task has a
   * priority higher than the currently running task.
   * @param item A reference to the item that is to be placed on the queue.
   * @retval true if the item was successfully posted
   * @retval false otherwise.
   *
   * <b>Example Usage</b>
   * @include Queue/sendToBackFromISR.cpp
   */
  inline bool sendToBackFromISR(bool& higherPriorityTaskWoken,
                                const T& item) const {
    BaseType_t taskWoken = pdFALSE;
    bool result =
        (xQueueSendToBackFromISR(handle, &item, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToBackFromISR( xQueue,
   * pvItemToQueue, pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueSendToBackFromISR.html>
   *
   * @overload
   */
  inline bool sendToBackFromISR(const T& item) const {
    return (xQueueSendToBackFromISR(handle, &item, NULL) == pdPASS);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToFront( xQueue, pvItemToQueue,
   * xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xQueueSendToFront.html>
   *
   * @param item A reference to the item that is to be placed on the queue.
   * @param ticksToWait The maximum amount of time the task should block waiting
   * for space to become available on the queue, should it already be full.  The
   * call will return immediately if this is set to 0 and the queue is full. The
   * time is defined in tick periods so the constant portTICK_PERIOD_MS should
   * be used to convert to real time if this is required.
   * @retval true if the item was successfully posted.
   * @retval false otherwise.
   *
   * <b>Example Usage</b>
   * @include Queue/sendToFront.cpp
   */
  inline bool sendToFront(const T& item,
                          const TickType_t ticksToWait = portMAX_DELAY) const {
    return (xQueueSendToFront(handle, &item, ticksToWait) == pdTRUE);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToFrontFromISR( xQueue,
   * pvItemToQueue, pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueSendToFrontFromISR.html>
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending to the queue caused a task to unblock, and the unblocked task has a
   * priority higher than the currently running task.
   * @param item A reference to the item that is to be placed on the queue.
   * @retval true if the item was successfully posted
   * @retval false otherwise.
   *
   * <b>Example Usage</b>
   * @include Queue/sendToFrontFromISR.cpp
   */
  inline bool sendToFrontFromISR(bool& higherPriorityTaskWoken,
                                 const T& item) const {
    BaseType_t taskWoken = pdFALSE;
    bool result =
        (xQueueSendToFrontFromISR(handle, &item, &taskWoken) == pdPASS);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>xQueueSendToFrontFromISR( xQueue,
   * pvItemToQueue, pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueSendToFrontFromISR.html>
   *
   * @overload
   */
  inline bool sendToFrontFromISR(const T& item) const {
    return (xQueueSendToFrontFromISR(handle, &item, NULL) == pdPASS);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueReceive( QueueHandle_t
   * xQueue, void *pvBuffer, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/a00118.html>
   *
   * Receive an item from a queue.  This function must not be used in an
   * interrupt service routine. See receiveFromISR() for an alternative that
   * can.
   *
   * @param ticksToWait The maximum amount of time the task should block waiting
   * for an item to receive should the queue be empty at the time of the call.
   * Setting ticksToWait to 0 will cause the function to return immediately if
   * the queue is empty. The time is defined in tick periods so the constant
   * portTICK_PERIOD_MS should be used to convert to real time if this is
   * required.
   * @return std::optional<T> Object from the queue. User should check that the
   * value is present.
   *
   * <b>Example Usage</b>
   * @include Queue/receive.cpp
   */
  inline std::optional<T> receive(
      const TickType_t ticksToWait = portMAX_DELAY) const {
    T buffer;
    return (xQueueReceive(handle, &buffer, ticksToWait) == pdTRUE)
               ? std::optional<T>(buffer)
               : std::nullopt;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueReceiveFromISR(
   * QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxHigherPriorityTaskWoken
   * )</tt>
   *
   * @see <https://www.freertos.org/a00120.html>
   *
   * Receive an item from a queue. It is safe to use this function from within
   * an interrupt service routine.
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending to the queue caused a task to unblock, and the unblocked task has a
   * priority higher than the currently running task.
   * @return std::optional<T> Object from the queue. User should check that the
   * value is present.
   *
   * <b>Example Usage</b>
   * @include Queue/receiveFromISR.cpp
   */
  inline std::optional<T> receiveFromISR(bool& higherPriorityTaskWoken) const {
    T buffer;
    BaseType_t taskWoken = pdFALSE;
    bool result = (xQueueReceiveFromISR(handle, &buffer, &taskWoken) == pdTRUE);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result ? std::optional<T>(buffer) : std::nullopt;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueReceiveFromISR(
   * QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxHigherPriorityTaskWoken
   * )</tt>
   *
   * @see <https://www.freertos.org/a00120.html>
   *
   * @overload
   */
  inline std::optional<T> receiveFromISR() const {
    T buffer;
    return (xQueueReceiveFromISR(handle, &buffer, NULL) == pdTRUE)
               ? std::optional<T>(buffer)
               : std::nullopt;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxQueueMessagesWaiting(
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#ucQueueMessagesWaiting>
   *
   * Return the number of messages stored in a queue.
   *
   * @retval UBaseType_t The number of messages available in the queue.
   */
  inline UBaseType_t messagesWaiting() const {
    return uxQueueMessagesWaiting(handle);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxQueueMessagesWaitingFromISR(
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#ucQueueMessagesWaitingFromISR>
   *
   * A version of messagesWaiting() that can be called from an ISR. Return the
   * number of messages stored in a queue.
   *
   * @retval UBaseType_t The number of messages available in the queue.
   */
  inline UBaseType_t messagesWaitingFromISR() const {
    return uxQueueMessagesWaitingFromISR(handle);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>UBaseType_t uxQueueSpacesAvailable(
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#uxQueueSpacesAvailable>
   *
   * Return the number of free spaces in a queue.
   *
   * @retval UBaseType_t The number of free spaces available in the queue.
   */
  inline UBaseType_t spacesAvailable() const {
    return uxQueueSpacesAvailable(handle);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueReset( QueueHandle_t xQueue
   * )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#xQueueReset>
   *
   * Resets a queue to its original empty state.
   */
  inline void reset() const { xQueueReset(handle); }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueOverwrite( QueueHandle_t
   * xQueue, const void * pvItemToQueue )</tt>
   *
   * @see <https://www.freertos.org/xQueueOverwrite.html>
   *
   * Only for use with queues that have a length of one - so the queue is either
   * empty or full.
   *
   * Post an item on a queue.  If the queue is already full then overwrite the
   * value held in the queue.  The item is queued by copy, not by reference.
   *
   * This function must not be called from an interrupt service routine. See
   * overwriteFromISR() for an alternative which may be used in an ISR.
   *
   * @param item A reference to the item that is to be placed on the queue.
   *
   * <b>Example Usage</b>
   * @include Queue/overwrite.cpp
   */
  inline void overwrite(const T& item) const { xQueueOverwrite(handle, &item); }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueOverwriteFromISR(
   * QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueOverwriteFromISR.html>
   *
   * A version of overwrite() that can be used in an interrupt service routine
   * (ISR).
   *
   * Only for use with queues that can hold a single item - so the queue is
   * either empty or full.
   *
   * Post an item on a queue.  If the queue is already full then overwrite the
   * value held in the queue.  The item is queued by copy, not by reference.
   *
   * @param higherPriorityTaskWoken A reference that will be set to true if
   * sending to the queue caused a task to unblock, and the unblocked task has a
   * priority higher than the currently running task.
   * @param item A reference to the item that is to be placed on the queue.
   *
   * <b>Example Usage</b>
   * @include Queue/overwriteFromISR.cpp
   */
  inline void overwriteFromISR(bool& higherPriorityTaskWoken,
                               const T& item) const {
    BaseType_t taskWoken = pdFALSE;
    xQueueOverwriteFromISR(handle, &item, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueOverwriteFromISR(
   * QueueHandle_t xQueue, const void * pvItemToQueue, BaseType_t
   * *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xQueueOverwriteFromISR.html>
   *
   * @overload
   */
  inline void overwriteFromISR(const T& item) const {
    xQueueOverwriteFromISR(handle, &item, NULL);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueuePeek( QueueHandle_t xQueue,
   * void * const pvBuffer, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xQueuePeek.html>
   *
   * Receive an item from a queue without removing the item from the queue.
   *
   * Successfully received items remain on the queue so will be returned again
   * by the next call, or a call to receive().
   *
   * This function must not be used in an interrupt service routine.  See
   * peekFromISR() for an alternative that can be called from an interrupt
   * service routine.
   *
   * @param ticksToWait The maximum amount of time the task should block waiting
   * for an item to receive should the queue be empty at the time of the call.
   * Setting ticksToWait to 0 will cause the function to return immediately if
   * the queue is empty. The time is defined in tick periods so the constant
   * portTICK_PERIOD_MS should be used to convert to real time if this is
   * required.
   * @return std::optional<T> Object from the queue. User should check that the
   * value is present.
   *
   * <b>Example Usage</b>
   * @include Queue/peek.cpp
   */
  inline std::optional<T> peek(
      const TickType_t ticksToWait = portMAX_DELAY) const {
    T buffer;
    return (xQueuePeek(handle, &buffer, ticksToWait) == pdTRUE)
               ? std::optional<T>(buffer)
               : std::nullopt;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueuePeekFromISR( QueueHandle_t
   * xQueue, void *pvBuffer )</tt>
   *
   * @see <https://www.freertos.org/xQueuePeekFromISR.html>
   *
   * A version of peek() that can be called from an interrupt service routine
   * (ISR).
   *
   * Receive an item from a queue without removing the item from the queue.
   *
   * Successfully received items remain on the queue so will be returned again
   * by the next call, or a call to receive().
   *
   * @return std::optional<T> Object from the queue. User should check that the
   * value is present.
   */
  inline std::optional<T> peekFromISR() const {
    T buffer;
    return (xQueuePeekFromISR(handle, &buffer) == pdTRUE)
               ? std::optional<T>(buffer)
               : std::nullopt;
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>void vQueueAddToRegistry( QueueHandle_t
   * xQueue, char *pcQueueName )</tt>
   *
   * @see <https://www.freertos.org/vQueueAddToRegistry.html>
   *
   * The registry is provided as a means for kernel aware debuggers to locate
   * queues, semaphores and mutexes.  Call addToRegistry() add a queue,
   * semaphore or mutex handle to the registry if you want the handle to be
   * available to a kernel aware debugger.  If you are not using a kernel aware
   * debugger then this function can be ignored.
   *
   * configQUEUE_REGISTRY_SIZE defines the maximum number of handles the
   * registry can hold.  configQUEUE_REGISTRY_SIZE must be greater than 0 within
   * FreeRTOSConfig.h for the registry to be available.  Its value does not
   * effect the number of queues, semaphores and mutexes that can be created -
   * just the number that the registry can hold.
   *
   * If addToRegistry() is called more than once for the same queue, the
   * registry will store the name parameter from the most recent call to
   * addToRegistry().
   *
   * @param name The name to be associated with the handle.  This is the name
   * that the kernel aware debugger will display.  The queue registry only
   * stores a pointer to the string - so the string must be persistent (global
   * or preferably in ROM/Flash), not on the stack.
   */
  inline void addToRegistry(const char* name) const {
    vQueueAddToRegistry(handle, name);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>void vQueueUnregisterQueue( QueueHandle_t
   * xQueue )</tt>
   *
   * @see <https://www.freertos.org/vQueueUnregisterQueue.html>
   *
   * The registry is provided as a means for kernel aware debuggers to locate
   * queues, semaphores and mutexes.  Call addToRegistry() add a queue,
   * semaphore or mutex handle to the registry if you want the handle to be
   * available to a kernel aware debugger, and unregister() to remove the queue,
   * semaphore or mutex from the register.  If you are not using a kernel aware
   * debugger then this function can be ignored.
   */
  inline void unregister() const { vQueueUnregisterQueue(handle); }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>const char *pcQueueGetName( QueueHandle_t
   * xQueue )</tt>
   *
   * @see <https://www.freertos.org/pcQueueGetName.html>
   *
   * The queue registry is provided as a means for kernel aware debuggers to
   * locate queues, semaphores and mutexes. Call getName() to look up and return
   * the name of a queue in the queue registry from the queue's handle.
   *
   * @return If the queue referenced by the queue is in the queue registry, then
   * the text name of the queue is returned, otherwise NULL is returned.
   */
  inline const char* getName() const { return pcQueueGetName(handle); }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueIsQueueFullFromISR( const
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#xQueueIsQueueFullFromISR>
   *
   * Queries a queue to determine if the queue is empty. This function should
   * only be used in an ISR.
   *
   * @return true if the queue is full.
   * @return false if the queue is not full.
   */
  inline bool isFullFromISR() const {
    return (xQueueIsQueueFullFromISR(handle) == pdTRUE);
  }

  /**
   * Queue.hpp
   *
   * @brief Function that calls <tt>BaseType_t xQueueIsQueueEmptyFromISR( const
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#xQueueIsQueueEmptyFromISR>
   *
   * Queries a queue to determine if the queue is empty. This function should
   * only be used in an ISR.
   *
   * @retval true if the queue is empty.
   * @retval false if the queue is not empty.
   */
  inline bool isEmptyFromISR() const {
    return (xQueueIsQueueEmptyFromISR(handle) == pdTRUE);
  }

 private:
  /**
   * Queue.hpp
   *
   * @brief Construct a new QueueBase object.
   *
   * @note Default constructor is deliberately private as this class is not
   * intended to be instantiated or derived from by the user. Use
   * FreeRTOS::Queue or FreeRTOS::StaticQueue.
   */
  QueueBase() = default;

  /**
   * Queue.hpp
   *
   * @brief Destroy the QueueBase object by calling <tt>void vQueueDelete(
   * QueueHandle_t xQueue )</tt>
   *
   * @see <https://www.freertos.org/a00018.html#vQueueDelete>
   *
   * Delete a queue - freeing all the memory allocated for storing of items
   * placed on the queue.
   */
  ~QueueBase() { vQueueDelete(this->handle); }

  QueueBase(QueueBase&&) noexcept = default;
  QueueBase& operator=(QueueBase&&) noexcept = default;

  /**
   * @brief Handle used to refer to the queue when using the FreeRTOS interface.
   */
  QueueHandle_t handle = NULL;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class Queue Queue.hpp <FreeRTOS/Queue.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS queue.
 *
 * Each queue requires RAM that is used to hold the queue state, and to hold the
 * items that are contained in the queue (the queue storage area). If a queue is
 * created using this class then the required RAM is automatically allocated
 * from the FreeRTOS heap.
 *
 * @tparam T Type to be stored in the queue.
 */
template <class T>
class Queue : public QueueBase<T> {
 public:
  /**
   * Queue.hpp
   *
   * @brief Construct a new Queue object by calling <tt>QueueHandle_t
   * xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize )</tt>
   *
   * @see <https://www.freertos.org/a00116.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * queue was created successfully in case the memory required to create the
   * queue could not be allocated.
   *
   * @param length The maximum number of items the queue can hold at any one
   * time.
   *
   * <b>Example Usage</b>
   * @include Queue/queue.cpp
   */
  explicit Queue(const UBaseType_t length) {
    this->handle = xQueueCreate(length, sizeof(T));
  }
  ~Queue() = default;

  Queue(const Queue&) = delete;
  Queue& operator=(const Queue&) = delete;

  Queue(Queue&&) noexcept = default;
  Queue& operator=(Queue&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticQueue Queue.hpp <FreeRTOS/Queue.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS queue.
 *
 * If a queue is created using this class then the RAM is provided by the
 * application writer as part of the object instance and allows the RAM to be
 * statically allocated at compile time.
 *
 * @tparam T Type to be stored in the queue.
 * @tparam N The maximum number of items the queue can hold at any one time.
 */
template <class T, UBaseType_t N>
class StaticQueue : public QueueBase<T> {
 public:
  /**
   *  Queue.hpp
   *
   * @brief Construct a new StaticQueue object by calling
   * <tt>QueueHandle_t xQueueCreateStatic( UBaseType_t uxQueueLength,
   * UBaseType_t uxItemSize, uint8_t *pucQueueStorageBuffer, StaticQueue_t
   * *pxQueueBuffer )</tt>
   *
   * @see <https://www.freertos.org/xQueueCreateStatic.html>
   *
   * @warning This class contains the storage buffer for the queue, so the user
   * should create this object as a global object or with the static storage
   * specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include Queue/staticQueue.cpp
   */
  StaticQueue() {
    this->handle = xQueueCreateStatic(N, sizeof(T), storage, &staticQueue);
  }
  ~StaticQueue() = default;

  StaticQueue(const StaticQueue&) = delete;
  StaticQueue& operator=(const StaticQueue&) = delete;

  StaticQueue(StaticQueue&&) noexcept = default;
  StaticQueue& operator=(StaticQueue&&) noexcept = default;

 private:
  StaticQueue_t staticQueue;
  uint8_t storage[N * sizeof(T)];
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_QUEUE_HPP
