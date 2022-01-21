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

void MyTask::taskFunction() {
  // Create another task.
  MyDifferentTask myDifferentTask;

  // ...

  // Suspend the created task.
  myDifferentTask.suspend();

  // ...

  // The created task will not run during this period, unless another task calls
  // myDifferentTask.resume().

  //...

  // Resume the suspended task ourselves.
  myDifferentTask.resume();

  // The created task will once again get microcontroller processing time in
  // accordance with its priority within the system.
}

void MyDifferentTask::taskFunction() {
  for (;;) {
    // ...
  }
}
