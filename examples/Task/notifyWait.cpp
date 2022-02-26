#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  void prvProcessBit0Event() {}
  void prvProcessBit1Event() {}
  void prvProcessBit2Event() {}
};

MyTask myTask;

// This task shows bits within the RTOS task notification value being used to
// pass different events to the task in the same way that flags in an event
// group might be used for the same purpose.
void MyTask::taskFunction() {
  for (;;) {
    // Block indefinitely (without a timeout, so no need to check the function's
    // return value) to wait for a notification.  Bits in this RTOS task's
    // notification value are set by the notifying tasks and interrupts to
    // indicate which events have occurred.
    auto notifiedValue = notifyWait().second;

    // Process any events that have been latched in the notified value.
    if (notifiedValue[0]) {
      // Bit 0 was set - process whichever event is represented by bit 0.
      prvProcessBit0Event();
    }

    if (notifiedValue[1]) {
      // Bit 1 was set - process whichever event is represented by bit 1.
      prvProcessBit1Event();
    }

    if (notifiedValue[2]) {
      // Bit 2 was set - process whichever event is represented by bit 2.
      prvProcessBit2Event();
    }
  }
}
