#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask0 : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyTask1 : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyTask2 : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class TaskManager {
 private:
  MyTask0 task0;
  MyTask1 task1;
  MyTask2 task2;
};

FreeRTOS::StaticEventGroup eventGroup;

void function() {
  TaskManager manager;
  FreeRTOS::Kernel::startScheduler();
}

constexpr FreeRTOS::EventGroup::EventBits task_0(1 << 0);
constexpr FreeRTOS::EventGroup::EventBits task_1(1 << 1);
constexpr FreeRTOS::EventGroup::EventBits task_2(1 << 2);
const FreeRTOS::EventGroup::EventBits all_tasks(task_0 | task_1 | task_2);

void MyTask0::taskFunction() {
  for (;;) {
    // Perform task functionality here.

    // Set bit 0 in the event group to note this task has reached the sync
    // point.  The other two tasks will set the other two bits defined by
    // all_tasks.  All three tasks have reached the synchronisation point when
    // all the all_tasks are set.  Wait a maximum of 100ms for this to happen.
    auto value = eventGroup.sync(task_0, all_tasks, pdMS_TO_TICKS(100));

    if ((value & all_tasks) == all_tasks) {
      // All three tasks reached the synchronisation point before the call to
      // sync() timed out.
    }
  }
}

void MyTask1::taskFunction() {
  for (;;) {
    // Perform task functionality here.

    // Set bit 1 in the event group to note this task has reached the
    // synchronisation point.  The other two tasks will set the other two bits
    // defined by all_tasks.  All three tasks have reached the synchronisation
    // point when all the all_tasks are set.  Wait indefinitely for this to
    // happen.
    eventGroup.sync(task_1, all_tasks);

    // sync() was called with an indefinite block time, so this task will only
    // reach here if the syncrhonisation was made by all three tasks, so there
    // is no need to test the return value.
  }
}

void MyTask2::taskFunction() {
  for (;;) {
    // Perform task functionality here.

    // Set bit 2 in the event group to note this task has reached the
    // synchronisation point.  The other two tasks will set the other two bits
    // defined by all_tasks.  All three tasks have reached the synchronisation
    // point when all the all_tasks are set.  Wait indefinitely for this to
    // happen.
    eventGroup.sync(task_1, all_tasks);

    // sync() was called with an indefinite block time, so this task will only
    // reach here if the syncrhonisation was made by all three tasks, so there
    // is no need to test the return value.
  }
}
