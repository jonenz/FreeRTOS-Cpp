#include <FreeRTOS/Mutex.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

class MyOtherTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

FreeRTOS::StaticMutex mutex;

// A task that uses the mutex.
void MyTask::taskFunction() {
  // Do something
}

// Another task that uses the mutex.
void MyOtherTask::taskFunction() {
  /// ... Do other things.

  if (mutex.isValid()) {
    // See if we can lock the mutex.  If the mutex is not available wait 10
    // ticks to see if it becomes free.
    if (mutex.lock()) {
      // We were able to obtain the mutex and can now access the shared
      // resource.

      // ...

      // We have finished accessing the shared resource.  Release the mutex.
      mutex.unlock();
    } else {
      // We could not lock the mutex and can therefore not access the shared
      // resource safely.
    }
  }
}
