#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  for (;;) {
    // Task code goes here.

    // ...

    // At some point the task wants to perform an operation during which it does
    // not want to get swapped out, or it wants to access data which is also
    // accessed from another task (but not from an interrupt).  It cannot use
    // FreeRTOS::Task::enterCritical() or FreeRTOS::Task::exitCritical() as the
    // length of the operation may cause interrupts to be missed.

    // Prevent the real time kernel swapping out the task.
    FreeRTOS::Kernel::suspendAll();

    // Perform the operation here.  There is no need to use critical sections as
    // we have all the microcontroller processing time.  During this time
    // interrupts will still operate and the kernel tick count will be
    // maintained.

    // ...

    // The operation is complete.  Restart the kernel.  We want to force a
    // context switch - but there is no point if resuming the scheduler caused a
    // context switch already.
    if (!FreeRTOS::Kernel::resumeAll()) {
      FreeRTOS::Kernel::yield();
    }
  }
}
