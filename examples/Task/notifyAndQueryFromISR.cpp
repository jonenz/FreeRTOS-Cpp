#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask myTask;

void MyTask::taskFunction() {
  for (;;) {
    // ...
  }
}

void anISR() {
  bool higherPriorityTaskWoken = false;

  // Set bit 8 in the 0th notification value. Store the task's previous 0th
  // notification value (before bit 8 is set).
  auto [updated, previousValue] = myTask.notifyAndQueryFromISR(
      higherPriorityTaskWoken, FreeRTOS::Task::NotifyAction::SetBits,
      (1UL << 8UL));

  // The task's previous notification value is saved in previousValue.

  // If the task was in the Blocked state, waiting for the notification, then it
  // will now have been moved from the Blocked state to the Ready state.  If its
  // priority is higher than the priority of the currently executing task (the
  // task this interrupt interrupted) then higherPriorityTaskWoken will have
  // been set to true, and passing the variable into a call to
  // FreeRTOS::Kernel::yield() will result in the interrupt returning directly
  // to the unblocked task.
  if (higherPriorityTaskWoken) {
    FreeRTOS::Kernel::yield();
  }
}
