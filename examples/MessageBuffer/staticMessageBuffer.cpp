#include <FreeRTOS/MessageBuffer.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  // Variable to hold the message buffer and it's underlying storage.
  static FreeRTOS::StaticMessageBuffer<1000> messageBuffer;

  // Other code that uses the message buffer can go here.
}
