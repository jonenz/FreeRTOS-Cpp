#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  FreeRTOS::CountingSemaphore countingSemaphore(10);

  // Create a counting semaphore that has a maximum count of 10 and an initial
  // count of 0.
  if (countingSemaphore.isValid()) {
    // The semaphore was created successfully.
  }

  // Rest of task code.
}
