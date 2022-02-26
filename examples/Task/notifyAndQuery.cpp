#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  for (;;) {
    // ...
  }
}

MyTask task1;
MyTask task2;
MyTask task3;
MyTask task4;

void aFunction() {
  using namespace FreeRTOS;

  Task::NotificationBits previousValue;

  // Set bit 8 in the 0th notification value of task1.  Store the task's
  // previous 0th notification value (before bit 8 is set) in previousValue.
  previousValue =
      task1.notifyAndQuery(Task::NotifyAction::SetBits, (1UL << 8UL)).second;

  // Send a notification to task2, potentially removing the task from the
  // Blocked state, but without updating the task's notification value.  Store
  // the tasks notification value in previousValue.
  previousValue = task2.notifyAndQuery(Task::NotifyAction::NoAction).second;

  // Set the notification value of task3 to 0x50, even if the task had not read
  // its previous notification value. The task's previous notification value is
  // of no interest.
  task3.notifyAndQuery(Task::NotifyAction::SetValueWithOverwrite, 0x50);

  // Set the notification value of task4 to 0xfff, but only if to do so would
  // not overwrite the task's existing notification value before the task had
  // obtained it (by a call to notifyWait() or notifyTake()).
  if (task4.notifyAndQuery(Task::NotifyAction::SetValueWithoutOverwrite, 0xfff)
          .first) {
    // The task's notification value was updated.
  } else {
    // The task's notification value was not updated.
  }
}
