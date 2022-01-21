#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>
#include <cstring>

class MyTask : public FreeRTOS::StaticTask<configMINIMAL_STACK_SIZE> {
 public:
  MyTask() : streamBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::StreamBuffer streamBuffer;
};

MyTask task;

void anInterruptServiceRoutine() {
  char const *pcStringToSend = "String to send";
  bool higherPriorityTaskWoken = false;

  // Attempt to send the string to the stream buffer.
  size_t xBytesSent = task.streamBuffer.sendFromISR(
      higherPriorityTaskWoken, pcStringToSend, sizeof(pcStringToSend));

  if (xBytesSent != sizeof(pcStringToSend)) {
    // There was not enough free space in the stream buffer for the entire
    // string to be written, ut xBytesSent bytes were written.
  }

  // If higherPriorityTaskWoken was set to true inside sendFromISR() then a task
  // that has a priority above the priority of the currently executing task was
  // unblocked and a context switch should be performed to ensure the ISR
  // returns to the unblocked task.  In most FreeRTOS ports this is done by
  // simply passing higherPriorityTaskWoken into FreeRTOS::yield(), which will
  // test the variables value, and perform the context switch if necessary.
  // Check the documentation for the port in use for port specific instructions.
  FreeRTOS::Kernel::yield();
}
