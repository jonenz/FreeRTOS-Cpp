#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>
#include <algorithm>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyDifferentTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

// Define a variable of type Message.  The examples below demonstrate how to
// pass the whole variable through the queue, and as the structure is moderately
// large, also how to pass a reference to the variable through a queue.
class Message {
 public:
  char ucMessageID;
  char ucData[20];
} xMessage;

// Queue used to send and receive complete Message objects.
FreeRTOS::StaticQueue<Message, 10> structQueue;

// Queue used to send and receive pointers to Message objects.
FreeRTOS::StaticQueue<Message*, 10> pointerQueue;

void MyTask::taskFunction() {
  xMessage.ucMessageID = static_cast<char>(0xab);
  std::fill_n(xMessage.ucData, 20, 0x12);

  // Send the entire object to the queue created to hold 10 objects.
  structQueue.sendToBack(xMessage);

  // Send the address of Message to the queue created to hold 10 pointers.
  pointerQueue.sendToBack(&xMessage);

  // ... Rest of task code goes here.
}

void MyDifferentTask::taskFunction() {
  // Receive a message from the created queue to hold a Message object.  Block
  // for 10 ticks if a message is not immediately available.  The value is read
  // into a message variable, so after calling receive() message will hold a
  // copy of xMessage.
  if (auto message = structQueue.receive(10)) {
    // message now contains a copy of xMessage.
  }

  // Receive a message from the created queue to hold pointers.  Block for 10
  // ticks if a message is not immediately available.  The value is read into a
  // pointer variable, and as the value received is the address of the xMessage
  // variable, after this call messagePointer will point to xMessage.
  if (auto messagePointer = pointerQueue.receive(10)) {
    // messagePointer now points to xMessage.
  }

  // ... Rest of task code goes here.
}
