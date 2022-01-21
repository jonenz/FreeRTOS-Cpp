#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  MyTask(const UBaseType_t priority, const char* name)
      : FreeRTOS::Task(priority, configMINIMAL_STACK_SIZE, name) {}
  void taskFunction() final;
};

// Task to be created.
void MyTask::taskFunction() {
  for (;;) {
    // Task code goes here.
  }
}

// Function that creates a task.
void aFunction() {
  // Create the task.
  MyTask task((tskIDLE_PRIORITY + 1), "NAME");

  // Check that the task was created successfully.
  if (task.isValid()) {
    // Start the scheduler.
    FreeRTOS::Kernel::startScheduler();
  }

  // Task will be destroyed when the scheduler is stopped and this function
  // returns.
}
