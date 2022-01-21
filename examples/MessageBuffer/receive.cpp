#include <FreeRTOS/MessageBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  MyTask() : messageBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::MessageBuffer messageBuffer;
};

void MyTask::taskFunction() {
  uint8_t ucRxData[20];
  size_t xReceivedBytes;

  // Receive the next message from the message buffer.  Wait in the Blocked
  // state (so not using any CPU processing time) for a maximum of 100ms for a
  // message to become available.
  xReceivedBytes = messageBuffer.receive((void*)ucRxData, sizeof(ucRxData),
                                         pdMS_TO_TICKS(20));

  if (xReceivedBytes > 0) {
    // A ucRxData contains a message that is xReceivedBytes long.  Process the
    // message here...
  }

  // Rest of task code.
}
