#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::StaticTask<configMINIMAL_STACK_SIZE> {
 public:
  MyTask(const UBaseType_t priority, const char* name)
      : FreeRTOS::StaticTask<configMINIMAL_STACK_SIZE>(priority, name) {}
  void taskFunction() final;
};

// Task to be created.
void MyTask::taskFunction() {
  for (;;) {
    // Task code goes here.
  }
}

// Create the task in persistent storage.
static MyTask task((tskIDLE_PRIORITY + 1), "NAME");

// Function that starts the scheduler.
void aFunction() {
  // Start the scheduler.
  FreeRTOS::Kernel::startScheduler();
}
