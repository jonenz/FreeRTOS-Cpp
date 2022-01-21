#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  FreeRTOS::BinarySemaphore binarySemaphore;

  if (binarySemaphore.isValid()) {
    if (binarySemaphore.give()) {
      // We would expect this call to fail because we cannot give a semaphore
      // without first "taking" it!
    }

    // Obtain the semaphore - don't block if the semaphore is not immediately
    // available.
    if (binarySemaphore.take(0)) {
      // We now have the semaphore and can access the shared resource.
      // ...
      // We have finished accessing the shared resource so can free the
      // semaphore.
      if (!binarySemaphore.give()) {
        // We would not expect this call to fail because we must have obtained
        // the semaphore to get here.
      }
    }
  }
}
