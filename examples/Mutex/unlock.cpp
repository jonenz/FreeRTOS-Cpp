#include <FreeRTOS/Mutex.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  FreeRTOS::Mutex mutex;

  if (mutex.isValid()) {
    if (mutex.unlock()) {
      // We would expect this call to fail because we cannot unlock a mutex
      // without first locking it!
    }

    // Lock the mutex - don't block if the mutex is not immediately available.
    if (mutex.lock(0)) {
      // We now have the mutex and can access the shared resource.
      // ...
      // We have finished accessing the shared resource so can unlock the mutex.
      if (!mutex.unlock()) {
        // We would not expect this call to fail because we must have obtained
        // the mutex to get here.
      }
    }
  }
}
