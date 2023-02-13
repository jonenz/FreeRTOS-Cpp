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

#ifndef FREERTOS_MESSAGEBUFFER_HPP
#define FREERTOS_MESSAGEBUFFER_HPP

#include "FreeRTOS.h"
#include "message_buffer.h"

namespace FreeRTOS {

/**
 * @class MessageBufferBase MessageBuffer.hpp <FreeRTOS/MessageBuffer.hpp>
 *
 * @brief Base class that provides the standard message buffer interface to
 * FreeRTOS::MessageBuffer and FreeRTOS::StaticMessageBuffer.
 *
 * @note This class is not intended to be instantiated by the user.  Use
 * FreeRTOS::MessageBuffer or FreeRTOS::StaticMessageBuffer.
 *
 * @warning Uniquely among FreeRTOS objects, the stream buffer implementation
 * (so also the message buffer implementation, as message buffers are built on
 * top of stream buffers) assumes there is only one task or interrupt that will
 * write to the buffer (the writer), and only one task or interrupt that will
 * read from the buffer (the reader).  It is safe for the writer and reader to
 * be different tasks or interrupts, but, unlike other FreeRTOS objects, it is
 * not safe to have multiple different writers or multiple different readers. If
 * there are to be multiple different writers then the application writer must
 * place each call to a writing API function (such as send()) inside a critical
 * section and set the send block time to 0.  Likewise, if there are to be
 * multiple different readers then the application writer must place each call
 * to a reading API function (such as read()) inside a critical section and set
 * the receive block time to 0.
 */
class MessageBufferBase {
 public:
  friend class MessageBuffer;
  template <size_t>
  friend class StaticMessageBuffer;

  MessageBufferBase(const MessageBufferBase&) = delete;
  MessageBufferBase& operator=(const MessageBufferBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that checks if the underlying message buffer handle is not
   * NULL. This should be used to ensure a message buffer has been created
   * correctly.
   *
   * @retval true If the handle is not NULL.
   * @retval false If the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferSend(
   * MessageBufferHandle_t xMessageBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferSend.html>
   *
   * Sends a discrete message to the message buffer.  The message can be any
   * length that fits within the buffer's free space, and is copied into the
   * buffer.
   *
   * Use send() to write to a message buffer from a task.  Use sendFromISR() to
   * write to a message buffer from an interrupt service routine (ISR).
   *
   * @param data A pointer to the message that is to be copied into the message
   * buffer.
   * @param length The length of the message.  That is, the number of bytes to
   * copy from data into the message buffer. When a message is written to the
   * message buffer an additional sizeof( size_t ) bytes are also written to
   * store the message's length.  sizeof( size_t ) is typically 4 bytes on a
   * 32-bit architecture, so on most 32-bit architecture setting length to 20
   * will reduce the free space in the message buffer by 24 bytes (20 bytes of
   * message data and 4 bytes to hold the message length).
   * @param ticksToWait The maximum amount of time the calling task should
   * remain in the Blocked state to wait for enough space to become available in
   * the message buffer, should the message buffer have insufficient space when
   * send() is called.  The calling task will never block if ticksToWait is
   * zero.  The block time is specified in tick periods, so the absolute time it
   * represents is dependent on the tick frequency.  The macro pdMS_TO_TICKS()
   * can be used to convert a time specified in milliseconds into a time
   * specified in ticks.  Setting ticksToWait to portMAX_DELAY will cause the
   * task to wait indefinitely (without timing out), provided
   * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h.  Tasks do not use any
   * CPU time when they are in the Blocked state.
   * @return size_t The number of bytes written to the message buffer.  If the
   * call to send() times out before there was enough space to write the message
   * into the message buffer then zero is returned.  If the call did not time
   * out then length is returned.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/send.cpp
   */
  inline size_t send(const void* data, const size_t length,
                     const TickType_t ticksToWait = portMAX_DELAY) const {
    return xMessageBufferSend(handle, data, length, ticksToWait);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferSendFromISR(
   * MessageBufferHandle_t xMessageBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferSendFromISR.html>
   *
   * Interrupt safe version of the API function that sends a discrete message to
   * the message buffer. The message can be any length that fits within the
   * buffer's free space, and is copied into the buffer.
   *
   * Use send() to write to a message buffer from a task. Use sendFromISR() to
   * write to a message buffer from an interrupt service routine (ISR).
   *
   * @param higherPriorityTaskWoken It is possible that a message buffer will
   * have a task blocked on it waiting for data. Calling sendFromISR() can make
   * data available, and so cause a task that was waiting for data to leave the
   * Blocked state. If calling sendFromISR() causes a task to leave the Blocked
   * state, and the unblocked task has a priority higher than the currently
   * executing task (the task that was interrupted), then, internally,
   * sendFromISR() will set higherPriorityTaskWoken to true. If sendFromISR()
   * sets this value to true, then normally a context switch should be performed
   * before the interrupt is exited. This will ensure that the interrupt returns
   * directly to the highest priority Ready state task. higherPriorityTaskWoken
   * should be set to false before it is passed into the function. See the code
   * example below for an example.
   * @param data A pointer to the message that is to be copied into the message
   * buffer.
   * @param length The length of the message. That is, the number of bytes to
   * copy from data into the message buffer. When a message is written to the
   * message buffer an additional sizeof( size_t ) bytes are also written to
   * store the message's length. sizeof( size_t ) is typically 4 bytes on a
   * 32-bit architecture, so on most 32-bit architecture setting length to 20
   * will reduce the free space in the message buffer by 24 bytes (20 bytes of
   * message data and 4 bytes to hold the message length).
   * @return size_t The number of bytes actually written to the message buffer.
   * If the message buffer didn't have enough free space for the message to be
   * stored then 0 is returned, otherwise length is returned.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/sendFromISR.cpp
   */
  inline size_t sendFromISR(bool& higherPriorityTaskWoken, const void* data,
                            const size_t length) const {
    BaseType_t taskWoken = pdFALSE;
    size_t result = xMessageBufferSendFromISR(handle, data, length, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferSendFromISR(
   * MessageBufferHandle_t xMessageBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferSendFromISR.html>
   *
   * @overload
   */
  inline size_t sendFromISR(const void* data, const size_t length) const {
    return xMessageBufferSendFromISR(handle, data, length, NULL);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferReceive(
   * MessageBufferHandle_t xMessageBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferReceive.html>
   *
   * Use receive() to read from a message buffer from a task.
   * UsereceiveFromISR() to read from a message buffer from an interrupt service
   * routine (ISR).
   *
   * @param buffer A pointer to the buffer into which the received message is to
   * be copied.
   * @param bufferLength The length of the buffer pointed to by the buffer
   * parameter.  This sets the maximum length of the message that can be
   * received.  If bufferLength is too small to hold the next message then the
   * message will be left in the message buffer and 0 will be returned.
   * @param ticksToWait The maximum amount of time the task should remain in the
   * Blocked state to wait for a message, should the message buffer be empty.
   * receive() will return immediately if ticksToWait is zero and the message
   * buffer is empty.  The block time is specified in tick periods, so the
   * absolute time it represents is dependent on the tick frequency.  The macro
   * pdMS_TO_TICKS() can be used to convert a time specified in milliseconds
   * into a time specified in ticks.  Setting ticksToWait to portMAX_DELAY will
   * cause the task to wait indefinitely (without timing out), provided
   * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h.  Tasks do not use any
   * CPU time when they are in the Blocked state.
   * @return size_t The length, in bytes, of the message read from the message
   * buffer, if any.  If receive() times out before a message became available
   * then zero is returned.  If the length of the message is greater than
   * bufferLength then the message will be left in the message buffer and zero
   * is returned.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/receive.cpp
   */
  inline size_t receive(void* buffer, const size_t bufferLength,
                        const TickType_t ticksToWait = portMAX_DELAY) const {
    return xMessageBufferReceive(handle, buffer, bufferLength, ticksToWait);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferReceiveFromISR(
   * MessageBufferHandle_t xMessageBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferReceiveFromISR.html>
   *
   * Use receive() to read from a message buffer from a task.
   * UsereceiveFromISR() to read from a message buffer from an interrupt service
   * routine (ISR).
   *
   * @param higherPriorityTaskWoken It is possible that a message buffer will
   * have a task blocked on it waiting for space to become available. Calling
   * receiveFromISR() can make space available, and so cause a task that is
   * waiting for space to leave the Blocked state. If calling receiveFromISR()
   * causes a task to leave the Blocked state, and the unblocked task has a
   * priority higher than the currently executing task (the task that was
   * interrupted), then, internally, receiveFromISR() will set
   * higherPriorityTaskWoken to true. If receiveFromISR() sets this value to
   * true, then normally a context switch should be performed before the
   * interrupt is exited. That will ensure the interrupt returns directly to the
   * highest priority Ready state task.  higherPriorityTaskWoken should be set
   * to false before it is passed into the function. See the code example below
   * for an example.
   * @param buffer A pointer to the buffer into which the received message is to
   * be copied.
   * @param bufferLength The length of the buffer pointed to by the buffer
   * parameter.  This sets the maximum length of the message that can be
   * received.  If bufferLength is too small to hold the next message then the
   * message will be left in the message buffer and 0 will be returned.
   * @return size_t The length, in bytes, of the message read from the message
   * buffer, if any.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/receiveFromISR.cpp
   */
  inline size_t receiveFromISR(bool& higherPriorityTaskWoken, void* buffer,
                               const size_t bufferLength) const {
    BaseType_t taskWoken = pdFALSE;
    size_t result =
        xMessageBufferReceiveFromISR(handle, buffer, bufferLength, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferReceiveFromISR(
   * MessageBufferHandle_t xMessageBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferReceiveFromISR.html>
   *
   * @overload
   */
  inline size_t receiveFromISR(void* buffer, const size_t bufferLength) const {
    return xMessageBufferReceiveFromISR(handle, buffer, bufferLength, NULL);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xMessageBufferSpacesAvailable(
   * MessageBufferHandle_t xMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferSpacesAvailable.html>
   *
   * Queries a message buffer to see how much free space it contains, which is
   * equal to the amount of data that can be sent to the message buffer before
   * it is full. The returned value is 4 bytes larger than the maximum message
   * size that can be sent to the message buffer.
   *
   * @return size_t The number of bytes that can be written to the message
   * buffer before the message buffer would be full. When a message is written
   * to the message buffer an additional sizeof( size_t ) bytes are also written
   * to store the message's length. sizeof( size_t ) is typically 4 bytes on a
   * 32-bit architecture, so if spacesAvailable() returns 10, then the size of
   * the largest message that can be written to the message buffer is 6 bytes.
   */
  inline size_t spacesAvailable() const {
    return xMessageBufferSpacesAvailable(handle);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xMessageBufferReset(
   * MessageBufferHandle_t xMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferReset.html>
   *
   * Resets a message buffer to its initial, empty, state. Any data that was in
   * the message buffer is discarded. A message buffer can only be reset if
   * there are no tasks blocked waiting to either send to or receive from the
   * message buffer.
   *
   * @retval true If the message buffer is reset.
   * @retval false If there was a task blocked waiting to send to or read from
   * the message buffer then the message buffer will not be reset.
   */
  inline bool reset() const { return (xMessageBufferReset(handle) == pdPASS); }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xMessageBufferIsEmpty(
   * MessageBufferHandle_t xMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferIsEmpty.html>
   *
   * Queries a message buffer to see if it is empty. A message buffer is empty
   * if it does not contain any messages.
   *
   * @retval true If the message buffer is empty.
   * @retval false Otherwise.
   */
  inline bool isEmpty() const {
    return (xMessageBufferIsEmpty(handle) == pdTRUE);
  }

  /**
   * MessageBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xMessageBufferIsFull(
   * MessageBufferHandle_t xMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferIsFull.html>
   *
   * Queries a message buffer to see if it is full. A message buffer is full if
   * it cannot accept any more messages, of any size, until space is made
   * available by a message being removed from the message buffer.
   *
   * @retval true If the message buffer is full.
   * @retval false Otherwise.
   */
  inline bool isFull() const {
    return (xMessageBufferIsFull(handle) == pdTRUE);
  }

 private:
  MessageBufferBase() = default;

  /**
   * MessageBuffer.hpp
   *
   * @brief Destroy the MessageBufferBase object by calling <tt>void
   * vMessageBufferDelete( MessageBufferHandle_t xMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/vMessageBufferDelete.html>
   *
   * Delete a queue - freeing all the memory allocated for storing of items
   * placed on the queue.
   */
  ~MessageBufferBase() { vMessageBufferDelete(this->handle); }

  MessageBufferBase(MessageBufferBase&&) noexcept = default;
  MessageBufferBase& operator=(MessageBufferBase&&) noexcept = default;

  MessageBufferHandle_t handle = NULL;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class MessageBuffer MessageBuffer.hpp <FreeRTOS/MessageBuffer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS message
 * buffer.
 *
 * A message buffer using dynamically allocated memory from the FreeRTOS heap.
 * See FreeRTOS::StaticMessageBuffer for a version that uses statically
 * allocated memory (memory that is allocated at compile time).
 */
class MessageBuffer : public MessageBufferBase {
 public:
  /**
   * MessageBuffer.hpp
   *
   * @brief Construct a new MessageBuffer object by calling
   * <tt>MessageBufferHandle_t xMessageBufferCreate( size_t xBufferSizeBytes
   * )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferCreate.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * message buffer was created successfully in case the memory required to
   * create the message buffer could not be allocated.
   *
   * @param size The total number of bytes (not messages) the message buffer
   * will be able to hold at any one time.  When a message is written to the
   * message buffer an additional sizeof( size_t ) bytes are also written to
   * store the message's length. sizeof( size_t ) is typically 4 bytes on a
   * 32-bit architecture, so on most 32-bit architectures a 10 byte message will
   * take up 14 bytes of message buffer space.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/messageBuffer.cpp
   */
  explicit MessageBuffer(size_t size) {
    this->handle = xMessageBufferCreate(size);
  }
  ~MessageBuffer() = default;

  MessageBuffer(const MessageBuffer&) = delete;
  MessageBuffer& operator=(const MessageBuffer&) = delete;

  MessageBuffer(MessageBuffer&&) noexcept = default;
  MessageBuffer& operator=(MessageBuffer&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticMessageBuffer MessageBuffer.hpp <FreeRTOS/MessageBuffer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS message
 * buffer.
 *
 * If a message buffer is created using this class then the RAM is provided by
 * the application writer as part of the object instance and allows the RAM to
 * be statically allocated at compile time.
 *
 * @tparam N The size, in bytes, of the storage for the message buffer.
 */
template <size_t N>
class StaticMessageBuffer : public MessageBufferBase {
 public:
  /**
   * MessageBuffer.hpp
   *
   * @brief Construct a new StaticMessageBuffer object by calling
   * <tt>MessageBufferHandle_t xMessageBufferCreateStatic( size_t
   * xBufferSizeBytes, uint8_t *pucMessageBufferStorageArea,
   * StaticMessageBuffer_t *pxStaticMessageBuffer )</tt>
   *
   * @see <https://www.freertos.org/xMessageBufferCreateStatic.html>
   *
   * @warning This class contains the storage buffer for the message buffer, so
   * the user should create this object as a global object or with the static
   * storage specifier so that the object instance is not on the stack.
   *
   * <b>Example Usage</b>
   * @include MessageBuffer/staticMessageBuffer.cpp
   */
  StaticMessageBuffer() : MessageBufferBase() {
    this->handle = xMessageBufferCreateStatic(sizeof(storage), storage,
                                              &staticMessageBuffer);
  }
  ~StaticMessageBuffer() = default;

  StaticMessageBuffer(const StaticMessageBuffer&) = delete;
  StaticMessageBuffer& operator=(const StaticMessageBuffer&) = delete;

  StaticMessageBuffer(StaticMessageBuffer&&) noexcept = default;
  StaticMessageBuffer& operator=(StaticMessageBuffer&&) noexcept = default;

 private:
  StaticMessageBuffer_t staticMessageBuffer;
  uint8_t storage[N] = {0};
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_MESSAGEBUFFER_HPP