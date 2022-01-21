#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::StaticTask<configMINIMAL_STACK_SIZE> {
 public:
  MyTask() : streamBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::StreamBuffer streamBuffer;
};

MyTask task;

void anInterruptServiceRoutine() {
  uint8_t ucRxData[20];
  size_t xReceivedBytes;
  bool higherPriorityTaskWoken = false;

  // Receive the next stream from the stream buffer.
  xReceivedBytes = task.streamBuffer.receiveFromISR(
      higherPriorityTaskWoken, (void*)ucRxData, sizeof(ucRxData));

  if (xReceivedBytes > 0) {
    // ucRxData contains xReceivedBytes read from the stream buffer.  Process
    // the stream here...
  }

  // If higherPriorityTaskWoken was set to true inside receiveFromISR() then a
  // task that has a priority above the priority of the currently executing task
  // was unblocked and a context switch should be performed to ensure the ISR
  // returns to the unblocked task.  In most FreeRTOS ports this is done by
  // simply passing higherPriorityTaskWoken into FreeRTOS::yield(), which will
  // test the variables value, and perform the context switch if necessary.
  // Check the documentation for the port in use for port specific instructions.
  FreeRTOS::Kernel::yield();
}
