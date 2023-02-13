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

#ifndef FREERTOS_STREAMBUFFER_HPP
#define FREERTOS_STREAMBUFFER_HPP

#include "FreeRTOS.h"
#include "stream_buffer.h"

namespace FreeRTOS {

/**
 * @class StreamBufferBase StreamBuffer.hpp <FreeRTOS/StreamBuffer.hpp>
 *
 * @brief Base class that provides the standard stream buffer interface to
 * FreeRTOS::StreamBuffer and FreeRTOS::StaticStreamBuffer.
 *
 * @note This class is not intended to be instantiated by the user.  Use
 * FreeRTOS::StreamBuffer or FreeRTOS::StaticStreamBuffer.
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
class StreamBufferBase {
 public:
  friend class StreamBuffer;
  template <size_t>
  friend class StaticStreamBuffer;

  StreamBufferBase(const StreamBufferBase&) = delete;
  StreamBufferBase& operator=(const StreamBufferBase&) = delete;

  static void* operator new(size_t, void* ptr) { return ptr; }
  static void* operator new[](size_t, void* ptr) { return ptr; }
  static void* operator new(size_t) = delete;
  static void* operator new[](size_t) = delete;

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that checks if the underlying stream buffer handle is not
   * NULL. This should be used to ensure a stream buffer has been created
   * correctly.
   *
   * @retval true If the handle is not NULL.
   * @retval false If the handle is NULL.
   */
  inline bool isValid() const { return (handle != NULL); }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferSend(
   * StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferSend.html>
   *
   * Sends bytes to a stream buffer. The bytes are copied into the stream
   * buffer.
   *
   * Use send() to write to a stream buffer from a task. Use sendFromISR() to
   * write to a stream buffer from an interrupt service routine (ISR).
   *
   * @param data A pointer to the buffer that holds the bytes to be copied into
   * the stream buffer.
   * @param length The maximum number of bytes to copy from data into the stream
   * buffer.
   * @param ticksToWait The maximum amount of time the task should remain in the
   * Blocked state to wait for enough space to become available in the stream
   * buffer, should the stream buffer contain too little space to hold the
   * another length bytes. The block time is specified in tick periods, so the
   * absolute time it represents is dependent on the tick frequency. The macro
   * pdMS_TO_TICKS() can be used to convert a time specified in milliseconds
   * into a time specified in ticks. Setting ticksToWait to portMAX_DELAY will
   * cause the task to wait indefinitely (without timing out), provided
   * INCLUDE_vTaskSuspend is set to 1 in FreeRTOSConfig.h. If a task times out
   * before it can write all length into the buffer it will still write as many
   * bytes as possible. A task does not use any CPU time when it is in the
   * blocked state.
   * @return size_t The number of bytes written to the stream buffer.  If a task
   * times out before it can write all length into the buffer it will still
   * write as many bytes as possible.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/send.cpp
   */
  inline size_t send(const void* data, const size_t length,
                     const TickType_t ticksToWait = portMAX_DELAY) const {
    return xStreamBufferSend(handle, data, length, ticksToWait);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferSendFromISR(
   * StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferSendFromISR.html>
   *
   * Interrupt safe version of the API function that sends a stream of bytes to
   * the stream buffer.
   *
   * Use send() to write to a stream buffer from a task. Use sendFromISR() to
   * write to a stream buffer from an interrupt service routine (ISR).
   *
   * @param higherPriorityTaskWoken It is possible that a stream buffer will
   * have a task blocked on it waiting for data. Calling sendFromISR() can make
   * data available, and so cause a task that was waiting for data to leave the
   * Blocked state.  If calling sendFromISR() causes a task to leave the Blocked
   * state, and the unblocked task has a priority higher than the currently
   * executing task (the task that was interrupted), then, internally,
   * sendFromISR() will set higherPriorityTaskWoken to true. If sendFromISR()
   * sets this value to true, then normally a context switch should be performed
   * before the interrupt is exited.  This will ensure that the interrupt
   * returns directly to the highest priority Ready state task.
   * higherPriorityTaskWoken should be set to false before it is passed into the
   * function. See the example code below for an example.
   * @param data A pointer to the buffer that holds the bytes to be copied into
   * the stream buffer.
   * @param length The maximum number of bytes to copy from data into the stream
   * buffer.
   * @return size_t The number of bytes written to the stream buffer.  If a task
   * times out before it can write all length into the buffer it will still
   * write as many bytes as possible.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/sendFromISR.cpp
   */
  inline size_t sendFromISR(bool& higherPriorityTaskWoken, const void* data,
                            const size_t length) const {
    BaseType_t taskWoken = pdFALSE;
    size_t result = xStreamBufferSendFromISR(handle, data, length, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferSendFromISR(
   * StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t
   * xDataLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferSendFromISR.html>
   *
   * @overload
   */
  inline size_t sendFromISR(const void* data, const size_t length) const {
    return xStreamBufferSendFromISR(handle, data, length, NULL);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferReceive(
   * StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, TickType_t xTicksToWait )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferReceive.html>
   *
   * Receives bytes from a stream buffer.
   *
   * Use receive() to read from a stream buffer from a task. Use
   * receiveFromISR() to read from a stream buffer from an interrupt service
   * routine (ISR).
   *
   * @param buffer A pointer to the buffer into which the received bytes will be
   * copied.
   * @param bufferLength The length of the buffer pointed to by the data
   * parameter. This sets the maximum number of bytes to receive in one call.
   * receive() will return as many bytes as possible up to a maximum set by
   * length.
   * @param ticksToWait The maximum amount of time the task should remain in the
   * Blocked state to wait for data to become available if the stream buffer is
   * empty. receive() will return immediately if ticksToWait is zero. The block
   * time is specified in tick periods, so the absolute time it represents is
   * dependent on the tick frequency. The macro pdMS_TO_TICKS() can be used to
   * convert a time specified in milliseconds into a time specified in ticks.
   * Setting ticksToWait to portMAX_DELAY will cause the task to wait
   * indefinitely (without timing out), provided INCLUDE_vTaskSuspend is set to
   * 1 in FreeRTOSConfig.h. A task does not use any CPU time when it is in the
   * Blocked state.
   * @return size_t The number of bytes read from the stream buffer. This will
   * be the number of bytes available up to a maximum of length.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/receive.cpp
   */
  inline size_t receive(void* buffer, const size_t bufferLength,
                        const TickType_t ticksToWait = portMAX_DELAY) const {
    return xStreamBufferReceive(handle, buffer, bufferLength, ticksToWait);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferReceiveFromISR(
   * StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferReceiveFromISR.html>
   *
   * An interrupt safe version of the API function that receives bytes from a
   * stream buffer.
   *
   * Use receive() to read from a stream buffer from a task. Use
   * receiveFromISR() to read from a stream buffer from an interrupt service
   * routine (ISR).
   *
   * @param higherPriorityTaskWoken It is possible that a stream buffer will
   * have a task blocked on it waiting for space to become available.  Calling
   * receiveFromISR() can make space available, and so cause a task that is
   * waiting for space to leave the Blocked state.  If calling receiveFromISR()
   * causes a task to leave the Blocked state, and the unblocked task has a
   * priority higher than the currently executing task (the task that was
   * interrupted), then, internally, receiveFromISR() will set
   * higherPriorityTaskWoken to true. If receiveFromISR() sets this value to
   * true, then normally a context switch should be performed before the
   * interrupt is exited. That will ensure the interrupt returns directly to the
   * highest priority Ready state task. higherPriorityTaskWoken should be set to
   * false before it is passed into the function. See the code example below for
   * an example.
   * @param buffer A pointer to the buffer into which the received bytes will be
   * copied.
   * @param bufferLength The length of the buffer pointed to by the buffer
   * parameter. This sets the maximum number of bytes to receive in one call.
   * receive() will return as many bytes as possible up to a maximum set by
   * length.
   * @return size_t The number of bytes read from the stream buffer, if any.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/receiveFromISR.cpp
   */
  inline size_t receiveFromISR(bool& higherPriorityTaskWoken, void* buffer,
                               const size_t bufferLength) const {
    BaseType_t taskWoken = pdFALSE;
    size_t result =
        xStreamBufferReceiveFromISR(handle, buffer, bufferLength, &taskWoken);
    if (taskWoken == pdTRUE) {
      higherPriorityTaskWoken = true;
    }
    return result;
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferReceiveFromISR(
   * StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t
   * xBufferLengthBytes, BaseType_t *pxHigherPriorityTaskWoken )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferReceiveFromISR.html>
   *
   * @overload
   */
  inline size_t receiveFromISR(void* buffer, const size_t bufferLength) const {
    return xStreamBufferReceiveFromISR(handle, buffer, bufferLength, NULL);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferBytesAvailable(
   * StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferBytesAvailable.html>
   *
   * Queries the stream buffer to see how much data it contains, which is equal
   * to the number of bytes that can be read from the stream buffer before the
   * stream buffer would be empty.
   *
   * @return size_t The number of bytes that can be read from the stream buffer
   * before the stream buffer would be empty.
   */
  inline size_t bytesAvailable() const {
    return xStreamBufferBytesAvailable(handle);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>size_t xStreamBufferSpacesAvailable(
   * StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferSpacesAvailable.html>
   *
   * Queries a stream buffer to see how much free space it contains, which is
   * equal to the amount of data that can be sent to the stream buffer before it
   * is full.
   *
   * @return size_t The number of bytes that can be written to the stream buffer
   * before the stream buffer would be full.
   */
  inline size_t spacesAvailable() const {
    return xStreamBufferSpacesAvailable(handle);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xStreamBufferSetTriggerLevel(
   * StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferSetTriggerLevel.html>
   *
   * A stream buffer's trigger level is the number of bytes that must be in the
   * stream buffer before a task that is blocked on the stream buffer to wait
   * for data is moved out of the blocked state. For example, if a task is
   * blocked on a read of an empty stream buffer that has a trigger level of 1
   * then the task will be unblocked when a single byte is written to the buffer
   * or the task's block time expires. As another example, if a task is blocked
   * on a read of an empty stream buffer that has a trigger level of 10 then the
   * task will not be unblocked until the stream buffer contains at least 10
   * bytes or the task's block time expires. If a reading task's block time
   * expires before the trigger level is reached then the task will still
   * receive however many bytes are actually available. Setting a trigger level
   * of 0 will result in a trigger level of 1 being used. It is not valid to
   * specify a trigger level that is greater than the buffer size.
   *
   * @param triggerLevel The new trigger level for the stream buffer.
   * @retval true If triggerLevel was less than or equal to the stream buffer's
   * length then the trigger level was updated.
   * @retval false Otherwise.
   */
  inline bool setTriggerLevel(const size_t triggerLevel = 0) const {
    return (xStreamBufferSetTriggerLevel(handle, triggerLevel) == pdTRUE);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xStreamBufferReset(
   * StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferReset.html>
   *
   * Resets a stream buffer to its initial, empty, state. Any data that was in
   * the stream buffer is discarded. A stream buffer can only be reset if there
   * are no tasks blocked waiting to either send to or receive from the stream
   * buffer.
   *
   * @return true If the stream buffer is reset.
   * @return false  If there was a task blocked waiting to send to or read from
   * the stream buffer then the stream buffer was not reset.
   */
  inline bool reset() const { return (xStreamBufferReset(handle) == pdPASS); }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xStreamBufferIsEmpty(
   * StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferIsEmpty.html>
   *
   * Queries a stream buffer to see if it is empty.  A stream buffer is empty if
   * it does not contain any data.
   *
   * @return true If the stream buffer is empty.
   * @return false Otherwise.
   */
  inline bool isEmpty() const {
    return (xStreamBufferIsEmpty(handle) == pdTRUE);
  }

  /**
   * StreamBuffer.hpp
   *
   * @brief Function that calls <tt>BaseType_t xStreamBufferIsFull(
   * StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferIsFull.html>
   *
   * Queries a stream buffer to see if it is full. A stream buffer is full if it
   * does not have any free space, and therefore cannot accept any more data.
   *
   * @return true If the stream buffer is full.
   * @return false Otherwise.
   */
  inline bool isFull() const { return (xStreamBufferIsFull(handle) == pdTRUE); }

 private:
  StreamBufferBase() = default;

  /**
   * StreamBuffer.hpp
   *
   * @brief Destroy the StreamBufferBase object by calling
   * <tt>void vStreamBufferDelete( StreamBufferHandle_t xStreamBuffer )</tt>
   *
   * @see <https://www.freertos.org/vStreamBufferDelete.html>
   *
   * Deletes a stream buffer and free the allocated memory.
   */
  ~StreamBufferBase() { vStreamBufferDelete(this->handle); }

  StreamBufferBase(StreamBufferBase&&) noexcept = default;
  StreamBufferBase& operator=(StreamBufferBase&&) noexcept = default;

  StreamBufferHandle_t handle = NULL;
};

#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)

/**
 * @class StreamBuffer StreamBuffer.hpp <FreeRTOS/StreamBuffer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS stream buffer.
 *
 * A stream buffer using dynamically allocated memory from the FreeRTOS heap.
 * See FreeRTOS::StaticStreamBuffer for a version that uses statically allocated
 * memory (memory that is allocated at compile time).
 */
class StreamBuffer : public StreamBufferBase {
 public:
  /**
   * StreamBuffer.hpp
   *
   * @brief Construct a new StreamBuffer object by calling
   * <tt>StreamBufferHandle_t xStreamBufferCreate( size_t xBufferSizeBytes,
   * size_t xTriggerLevelBytes )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferCreate.html>
   *
   * @warning The user should call isValid() on this object to verify that the
   * stream buffer was created successfully in case the memory required to
   * create the message buffer could not be allocated.
   *
   * @param size The total number of bytes the stream buffer will be able to
   * hold at any one time.
   * @param triggerLevel 	The number of bytes that must be in the stream
   * buffer before a task that is blocked on the stream buffer to wait for data
   * is moved out of the blocked state. For example, if a task is blocked on a
   * read of an empty stream buffer that has a trigger level of 1 then the task
   * will be unblocked when a single byte is written to the buffer or the task's
   * block time expires. As another example, if a task is blocked on a read of
   * an empty stream buffer that has a trigger level of 10 then the task will
   * not be unblocked until the stream buffer contains at least 10 bytes or the
   * task's block time expires. If a reading task's block time expires before
   * the trigger level is reached then the task will still receive however many
   * bytes are actually available. Setting a trigger level of 0 will result in a
   * trigger level of 1 being used. It is not valid to specify a trigger level
   * that is greater than the buffer size.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/streamBuffer.cpp
   */
  explicit StreamBuffer(const size_t size, const size_t triggerLevel = 0) {
    this->handle = xStreamBufferCreate(size, triggerLevel);
  }
  ~StreamBuffer() = default;

  StreamBuffer(const StreamBuffer&) = delete;
  StreamBuffer& operator=(const StreamBuffer&) = delete;

  StreamBuffer(StreamBuffer&&) noexcept = default;
  StreamBuffer& operator=(StreamBuffer&&) noexcept = default;
};

#endif /* configSUPPORT_DYNAMIC_ALLOCATION */

#if (configSUPPORT_STATIC_ALLOCATION == 1)

/**
 * @class StaticStreamBuffer StreamBuffer.hpp <FreeRTOS/StreamBuffer.hpp>
 *
 * @brief Class that encapsulates the functionality of a FreeRTOS stream buffer.
 *
 * If a stream buffer is created using this class then the RAM is provided by
 * the application writer as part of the object instance and allows the RAM to
 * be statically allocated at compile time.
 *
 * @tparam N The size, in bytes, of the storage buffer for the stream buffer.
 */
template <size_t N>
class StaticStreamBuffer : public StreamBufferBase {
 public:
  /**
   * StreamBuffer.hpp
   *
   * @brief Construct a new StaticStreamBuffer object by calling
   * <tt>StreamBufferHandle_t xStreamBufferCreateStatic( size_t
   * xBufferSizeBytes, size_t xTriggerLevelBytes, uint8_t
   * *pucStreamBufferStorageArea, StaticStreamBuffer_t *pxStaticStreamBuffer
   * )</tt>
   *
   * @see <https://www.freertos.org/xStreamBufferCreateStatic.html>
   *
   * @param triggerLevel 	The number of bytes that must be in the stream
   * buffer before a task that is blocked on the stream buffer to wait for data
   * is moved out of the blocked state. For example, if a task is blocked on a
   * read of an empty stream buffer that has a trigger level of 1 then the task
   * will be unblocked when a single byte is written to the buffer or the task's
   * block time expires. As another example, if a task is blocked on a read of
   * an empty stream buffer that has a trigger level of 10 then the task will
   * not be unblocked until the stream buffer contains at least 10 bytes or the
   * task's block time expires. If a reading task's block time expires before
   * the trigger level is reached then the task will still receive however many
   * bytes are actually available. Setting a trigger level of 0 will result in a
   * trigger level of 1 being used. It is not valid to specify a trigger level
   * that is greater than the buffer size.
   *
   * <b>Example Usage</b>
   * @include StreamBuffer/staticStreamBuffer.cpp
   */
  explicit StaticStreamBuffer(const size_t triggerLevel = 0) {
    this->handle = xStreamBufferCreateStatic(sizeof(storage), triggerLevel,
                                             storage, &staticStreamBuffer);
  }
  ~StaticStreamBuffer() = default;

  StaticStreamBuffer(const StaticStreamBuffer&) = delete;
  StaticStreamBuffer& operator=(const StaticStreamBuffer&) = delete;

  StaticStreamBuffer(StaticStreamBuffer&&) noexcept = default;
  StaticStreamBuffer& operator=(StaticStreamBuffer&&) noexcept = default;

 private:
  StaticStreamBuffer_t staticStreamBuffer;
  uint8_t storage[N];
};

#endif /* configSUPPORT_STATIC_ALLOCATION */

}  // namespace FreeRTOS

#endif  // FREERTOS_STREAMBUFFER_HPP