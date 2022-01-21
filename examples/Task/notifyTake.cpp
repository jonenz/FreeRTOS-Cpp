#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

// Fake peripherial interface functions
BaseType_t xQueryPeripheral() { return 0; }
void vProcessPeripheralEvent(BaseType_t xEvent) {}
constexpr BaseType_t NO_MORE_EVENTS = 0;

MyTask myTask;

// An interrupt handler.  The interrupt handler does not perform any processing,
// instead it unblocks a high priority task in which the event that generated
// the interrupt is processed.  If the priority of the task is high enough then
// the interrupt will return directly to the task (so it will interrupt one task
// but return to a different task), so the processing will occur contiguously in
// time - just as if all the processing had been done in the interrupt handler
// itself.
void anInterruptHandler() {
  bool higherPriorityTaskWoken = false;

  // Unblock the handling task so the task can perform any processing
  // necessitated by the interrupt.
  myTask.notifyGiveFromISR(higherPriorityTaskWoken);

  if (higherPriorityTaskWoken) {
    // Force a context switch if notifyGiveFromISR was set to true.
    FreeRTOS::Kernel::yield();
  }
}

// A task that blocks waiting to be notified that the peripheral needs
// servicing, processing all the events pending in the peripheral each time it
// is notified to do so.
void MyTask::taskFunction() {
  BaseType_t xEvent;

  for (;;) {
    // Block indefinitely (without a timeout, so no need to check the function's
    // return value) to wait for a notification.  Here the RTOS task
    // notification is being used as a binary semaphore, so the notification
    // value is cleared to zero on exit.  NOTE!  Real applications should not
    // block indefinitely, but instead time out occasionally in order to handle
    // error conditions that may prevent the interrupt from sending any more
    // notifications.
    notifyTake();

    // The RTOS task notification is used as a binary (as opposed to a counting)
    // semaphore, so only go back to wait for further notifications when all
    // events pending in the peripheral have been processed.
    do {
      xEvent = xQueryPeripheral();

      if (xEvent != NO_MORE_EVENTS) {
        vProcessPeripheralEvent(xEvent);
      }

    } while (xEvent != NO_MORE_EVENTS);
  }
}
