#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Queue.hpp>

// Create a queue to hold one unsigned long value.  It is strongly recommended
// not to use overwriteFromISR() on queues that can contain more than one value,
// and doing so will trigger an assertion if configASSERT() is defined.
FreeRTOS::StaticQueue<uint64_t, 1> queue;

void anInterruptHandler() {
  bool higherPriorityTaskWoken = false;

  // Write the value 10 to the queue using overwriteFromISR().
  queue.overwriteFromISR(higherPriorityTaskWoken, 10);

  // The queue is full, but calling overwriteFromISR() again will still pass
  // because the value held in the queue will be overwritten with the new value.
  queue.overwriteFromISR(higherPriorityTaskWoken, 100);

  // Reading from the queue will now return 100.

  // ...

  if (higherPriorityTaskWoken) {
    // Writing to the queue caused a task to unblock and the unblocked task has
    // a priority higher than or equal to the priority of the currently
    // executing task (the task this interrupt interrupted).  Perform a context
    // switch so this interrupt returns directly to the unblocked task.
    FreeRTOS::Kernel::yield();
  }
}