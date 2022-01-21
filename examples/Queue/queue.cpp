#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class Message {
 public:
  char ucMessageID;
  char ucData[20];
} xMessage;

void MyTask::taskFunction() {
  // Create a queue capable of containing 10 unsigned long values.
  FreeRTOS::Queue<uint64_t> queue1(10);

  if (!queue1.isValid()) {
    // Queue was not created and must not be used.
  }

  // Create a queue capable of containing 10 pointers to Message objects.  These
  // should be passed by pointer as they contain a lot of data
  FreeRTOS::Queue<Message *> queue2(10);

  if (!queue2.isValid()) {
    // Queue was not created and must not be used.
  }

  // Rest of task code.
}
