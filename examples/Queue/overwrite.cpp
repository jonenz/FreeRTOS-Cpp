#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  // Create a queue to hold one unsigned long value.  It is strongly recommended
  // *not* to use overwrite() on queues that can contain more than one value,
  // and doing so will trigger an assertion if configASSERT() is defined.
  FreeRTOS::Queue<uint64_t> queue(1);

  if (!queue.isValid()) {
    // Queue was not created and must not be used.
  }

  // Write the value 10 to the queue using overwrite().
  queue.overwrite(10);

  // Peeking the queue should now return 10, but leave the value 10 in the
  // queue.  A block time of zero is used as it is known that the queue holds a
  // value.
  if (queue.peek().value() != 10) {
    // Error, unless another task removed the value.
  }

  // The queue is still full.  Use overwrite() to overwrite the value held in
  // the queue with 100.
  queue.overwrite(100);

  // This time read from the queue, leaving the queue empty once more.  A block
  // time of 0 is used again. The value read should be the last value written,
  // even though the queue was already full when the value was written.
  if (queue.receive().value() != 100) {
    // Error unless another task is using the same queue.
  }

  // ...
}
