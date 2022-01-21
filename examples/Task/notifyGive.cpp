#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyDifferentTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask task1;

MyDifferentTask task2;

void MyTask::taskFunction() {
  for (;;) {
    // Send notification to task2, bringing it out of the  Blocked state.
    task2.notifyGive();

    // Block to wait for task2 to notify this task.
    notifyTake(portMAX_DELAY, true);
  }
}

void MyDifferentTask::taskFunction() {
  for (;;) {
    // Block to wait for task1 to notify this task.
    notifyTake(portMAX_DELAY, true);

    // Send a notification to task1, bringing it out of the Blocked state.
    task1.notifyGive();
  }
}
