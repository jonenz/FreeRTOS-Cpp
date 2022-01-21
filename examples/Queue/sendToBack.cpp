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

const uint64_t ulVar = 10UL;

void MyTask::taskFunction() {
  // Create a queue capable of containing 10 unsigned long values.
  FreeRTOS::Queue<uint64_t> queue1(10);

  // Create a queue capable of containing 10 pointers to Message objects.  These
  // should be passed by pointer as they contain a lot of data
  FreeRTOS::Queue<Message *> queue2(10);

  // ...

  if (queue1.isValid()) {
    // Send an unsigned long.  Wait for 10 ticks for space to become available
    // if necessary.
    if (!queue1.sendToBack(ulVar, 10)) {
      // Failed to post the message, even after 10 ticks.
    }
  }

  if (queue2.isValid()) {
    // Send a pointer to a Message object.  Don't block if the queue is already
    // full.
    queue2.sendToBack(&xMessage, 0);
  }

  // Rest of task code.
}
