#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  FreeRTOS::BinarySemaphore binarySemaphore;

  if (binarySemaphore.isValid()) {
    // The semaphore was created successfully.
    // The semaphore can now be used.
  }

  // Rest of task code.
}
