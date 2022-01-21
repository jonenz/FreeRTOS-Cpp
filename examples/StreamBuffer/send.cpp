#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>
#include <cstring>

class MyTask : public FreeRTOS::Task {
 public:
  MyTask() : streamBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::StreamBuffer streamBuffer;
};

void MyTask::taskFunction() {
  size_t xBytesSent;
  uint8_t ucArrayToSend[] = {0, 1, 2, 3};
  char const *pcStringToSend = "String to send";

  // Send an array to the stream buffer, blocking for a maximum of 100ms to wait
  // for enough space to be available in the stream buffer.
  xBytesSent = streamBuffer.send(ucArrayToSend, sizeof(ucArrayToSend),
                                 pdMS_TO_TICKS(100));

  if (xBytesSent != sizeof(ucArrayToSend)) {
    // The call to send() times out before there was enough space in the buffer
    // for the data to be written, but it did successfully write xBytesSent
    // bytes.
  }

  // Send the string to the stream buffer.  Return immediately if there is not
  // enough space in the buffer.
  xBytesSent = streamBuffer.send(pcStringToSend, strlen(pcStringToSend), 0);

  if (xBytesSent != strlen(pcStringToSend)) {
    // The entire string could not be added to the stream buffer because there
    // was not enough free space in the buffer, but xBytesSent bytes were sent.
    // Could try again to send the remaining bytes.
  }

  // Rest of task code.
}
