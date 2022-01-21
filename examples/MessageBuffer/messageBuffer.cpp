#include <FreeRTOS/MessageBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  constexpr size_t messageBufferSize = 100;

  // Create a message buffer that can hold 100 bytes.  The memory used to hold
  // both the message buffer structure and the data in the message buffer is
  // allocated dynamically.
  FreeRTOS::MessageBuffer messageBuffer(messageBufferSize);

  if (messageBuffer.isValid()) {
    // The message buffer was created successfully and can now be used.
  } else {
    // There was not enough heap memory space available to create the message
    // buffer.
  }

  // Rest of task code.
}
