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
  // Raise the priority of this task.
  setPriority(tskIDLE_PRIORITY + 1);
}

void MyDifferentTask::taskFunction() {
  // Raise the priority of myTask.
  myTask.setPriority(myTask.getPriority() + 1);
}
