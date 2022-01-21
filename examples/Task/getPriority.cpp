#include <FreeRTOS/Task.hpp>

// Define a task that requries the priority to be specified.
class MyTask : public FreeRTOS::Task {
 public:
  explicit MyTask(UBaseType_t priority) : FreeRTOS::Task(priority) {}
  void taskFunction() final;
};

// Define a task that uses the default task priority.
class MyDifferentTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask myTask(tskIDLE_PRIORITY);

MyDifferentTask myDifferentTask;

void MyTask::taskFunction() {
  // Some code that may change the priority.

  // Obtain the priority of the this task.  It was created with
  // tskIDLE_PRIORITY, but it may have been changed.
  if (getPriority() != tskIDLE_PRIORITY) {
    // This task has changed priority.
  }
}

void MyDifferentTask::taskFunction() {
  // Obtain the priority of myTask.  It was created with tskIDLE_PRIORITY, but
  // it may have been changed.
  if (myTask.getPriority() != tskIDLE_PRIORITY) {
    // myTask has changed priority.
  }
}
