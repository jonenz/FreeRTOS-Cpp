#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/MessageBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::StaticTask<configMINIMAL_STACK_SIZE> {
 public:
  MyTask() : messageBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::MessageBuffer messageBuffer;
};

MyTask task;

void anInterruptServiceRoutine() {
  uint8_t ucRxData[20];
  size_t xReceivedBytes;
  bool higherPriorityTaskWoken = false;

  // Receive the next message from the message buffer.
  xReceivedBytes = task.messageBuffer.receiveFromISR(
      higherPriorityTaskWoken, (void*)ucRxData, sizeof(ucRxData));

  if (xReceivedBytes > 0) {
    // ucRxData contains a message that is xReceivedBytes long.  Process the
    // message here...
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
