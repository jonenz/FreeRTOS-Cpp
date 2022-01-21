#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  void prvProcessRxInterrupt() {}
  void prvProcessTxInterrupt() {}
  void prvClearBufferOverrun() {}
};

// Fake peripherial interface functions.
uint32_t ulReadPeripheralInterruptStatus() { return 0; }
void vClearPeripheralInterruptStatus(uint32_t ulStatusRegister) {}

MyTask myTask;

// The interrupt handler does not perform any processing itself.  Instead it
// unblocks a high priority task in which the events that generated the
// interrupt are processed.  If the priority of the task is high enough then the
// interrupt will return directly to the task (so it will interrupt one task but
// return to a different task), so the processing will occur contiguously in
// time - just as if all the processing had been done in the interrupt handler
// itself. The status of the interrupting peripheral is sent to the task using
// an RTOS task notification.
void anInterruptHandler() {
  bool higherPriorityTaskWoken = false;

  // Read the interrupt status register which has a bit for each interrupt
  // source (for example, maybe an Rx bit, a Tx bit, a buffer overrun bit, etc.
  uint32_t ulStatusRegister = ulReadPeripheralInterruptStatus();

  // Clear the interrupts.
  vClearPeripheralInterruptStatus(ulStatusRegister);

  // Unblock the task so the task can perform any processing necessitated by the
  // interrupt.  The task's 0th notification value is bitwise ORed with the
  // interrupt status - ensuring bits that are already set are not overwritten.
  myTask.notifyFromISR(higherPriorityTaskWoken,
                       FreeRTOS::Task::NotifyAction::SetBits, ulStatusRegister);

  if (higherPriorityTaskWoken) {
    // Force a context switch if higherPriorityTaskWoken is true.
    FreeRTOS::Kernel::yield();
  }
}

// A task that blocks waiting to be notified that the peripheral needs
// servicing, processing all the events pending in the peripheral each time it
// is notified to do so.
void MyTask::taskFunction() {
  for (;;) {
    // Block indefinitely (without a timeout, so no need to check the function's
    // return value) to wait for a notification.  NOTE!  Real applications
    // should not block indefinitely, but instead time out occasionally in order
    // to handle error conditions that may prevent the interrupt from sending
    // any more notifications.
    auto interruptStatus = std::get<1>(notifyWait());

    // Process any bits set in the received notification value.  This assumes
    // the peripheral sets bit 0 for an Rx interrupt, bit 1 for a Tx interrupt,
    // and bit 2 for a buffer overrun interrupt.
    if (interruptStatus[0]) {
      prvProcessRxInterrupt();
    }

    if (interruptStatus[1]) {
      prvProcessTxInterrupt();
    }

    if (interruptStatus[2]) {
      prvClearBufferOverrun();
    }
  }
}
