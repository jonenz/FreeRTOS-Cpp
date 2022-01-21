#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

// Create a counting semaphore that has a maximum count of 10 and an initial
// count of 0.
static FreeRTOS::StaticCountingSemaphore countingSemaphore(10);

void MyTask::taskFunction() {
  // Use the semaphore.

  // Rest of task code.
}
