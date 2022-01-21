#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  FreeRTOS::EventGroup eventGroup;
};

MyTask myTask;

void MyTask::taskFunction() {
  FreeRTOS::EventGroup::EventBits bitsToWaitFor;
  bitsToWaitFor[0] = true;
  bitsToWaitFor[4] = true;

  // Wait a maximum of 100ms for either bit 0 or bit 4 to be set within the
  // event group.  Clear the bits before exiting.
  auto bits = eventGroup.wait(bitsToWaitFor, true, false, pdMS_TO_TICKS(100));

  if (bits[0] && bits[4]) {
    // wait() returned because both bits were set.
  } else if (bits[0]) {
    // wait() returned because just bit 0 was set.
  } else if (bits[4]) {
    // wait() returned because just bit 4 was set.
  } else {
    // wait() returned because ticksToWait ticks passed without either bit 0 or
    // bit 4 becoming set.
  }
}
