#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyOtherTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

FreeRTOS::StaticBinarySemaphore binarySemaphore;

// A task that uses the semaphore.
void MyTask::taskFunction() {
  // Do something
}

// Another task that uses the semaphore.
void MyOtherTask::taskFunction() {
  /// ... Do other things.

  if (binarySemaphore.isValid()) {
    // See if we can obtain the semaphore.  If the semaphore is not available
    // wait 10 ticks to see if it becomes free.
    if (binarySemaphore.take()) {
      // We were able to obtain the semaphore and can now access the shared
      // resource.

      // ...

      // We have finished accessing the shared resource.  Release the semaphore.
      binarySemaphore.give();
    } else {
      // We could not obtain the semaphore and can therefore not access the
      // shared resource safely.
    }
  }
}
