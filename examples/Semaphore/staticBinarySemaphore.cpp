#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

// Create a binary semaphore without using any dynamic memory allocation.
static FreeRTOS::StaticBinarySemaphore binarySemaphore;

void MyTask::taskFunction() {
  // Use the semaphore.

  // Rest of task code.
}
