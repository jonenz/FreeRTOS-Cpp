#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  constexpr size_t streamBufferSize = 100;

  // Create a stream buffer that can hold 100 bytes.  The memory used to hold
  // both the stream buffer structure and the data in the stream buffer is
  // allocated dynamically.
  FreeRTOS::StreamBuffer streamBuffer(streamBufferSize);

  if (streamBuffer.isValid()) {
    // The stream buffer was created successfully and can now be used.
  } else {
    // There was not enough heap memory space available to create the stream
    // buffer.
  }

  // Rest of task code.
}
