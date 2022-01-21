#include <FreeRTOS/MessageBuffer.hpp>
#include <FreeRTOS/Task.hpp>
#include <cstring>

class MyTask : public FreeRTOS::Task {
 public:
  MyTask() : messageBuffer(100) {}
  void taskFunction() final;
  FreeRTOS::MessageBuffer messageBuffer;
};

void MyTask::taskFunction() {
  size_t xBytesSent;
  uint8_t ucArrayToSend[] = {0, 1, 2, 3};
  char const *pcStringToSend = "String to send";

  // Send an array to the message buffer, blocking for a maximum of 100ms to
  // wait for enough space to be available in the message buffer.
  xBytesSent = messageBuffer.send(ucArrayToSend, sizeof(ucArrayToSend),
                                  pdMS_TO_TICKS(100));

  if (xBytesSent != sizeof(ucArrayToSend)) {
    // The call to send() times out before there was enough space in the buffer
    // for the data to be written.
  }

  // Send the string to the message buffer.  Return immediately if there is not
  // enough space in the buffer.
  xBytesSent = messageBuffer.send(pcStringToSend, strlen(pcStringToSend), 0);

  if (xBytesSent != strlen(pcStringToSend)) {
    // The string could not be added to the message buffer because there was not
    // enough free space in the buffer.
  }

  // Rest of task code.
}
