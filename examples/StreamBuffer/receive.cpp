#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  MyTask() : streamBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::StreamBuffer streamBuffer;
};

void MyTask::taskFunction() {
  uint8_t ucRxData[20];
  size_t xReceivedBytes;

  // Receive up to another sizeof( ucRxData ) bytes from the stream buffer. Wait
  // in the Blocked state (so not using any CPU processing time) for a maximum
  // of 100ms for the full sizeof( ucRxData ) number of bytes to be available.
  xReceivedBytes = streamBuffer.receive((void*)ucRxData, sizeof(ucRxData),
                                        pdMS_TO_TICKS(20));

  if (xReceivedBytes > 0) {
    // A ucRxData contains another xRecievedBytes bytes of data, which can be
    // processed here...
  }

  // Rest of task code.
}
