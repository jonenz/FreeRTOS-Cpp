#include <FreeRTOS/StreamBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  // Variable to hold the stream buffer and it's underlying storage.
  static FreeRTOS::StaticStreamBuffer<1000> streamBuffer;

  // Other code that uses the message buffer can go here.
}
