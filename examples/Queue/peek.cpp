#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyDifferentTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class Message {
 public:
  char ucMessageID;
  char ucData[20];
} xMessage;

// Queue used to send and receive pointers to Message objects.
FreeRTOS::StaticQueue<Message*, 10> pointerQueue;

void MyTask::taskFunction() {
  // Send a pointer to a Message object.  Don't block if the queue is already
  // full.
  pointerQueue.sendToBack(&xMessage);

  // ... Rest of task code goes here.
}

void MyDifferentTask::taskFunction() {
  // Peek a message on the created queue.  Block for 10 ticks if a message is
  // not immediately available.
  if (auto messagePointer = pointerQueue.peek(10)) {
    // messagePointer now points to the xMessage variable posted by MyTask, but
    // the item still remains on the queue.
  }

  // ... Rest of task code goes here.
}
