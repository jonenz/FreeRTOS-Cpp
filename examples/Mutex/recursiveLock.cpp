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

      // For some reason due to the nature of the code further calls to take()
      // are made on the same mutex.  In real code these would not be just
      // sequential calls as this would make no sense.  Instead the calls are
      // likely to be buried inside a more complex call structure.
      mutex.lock(10);
      mutex.lock(10);

      // The mutex has now been locked three times, so will not be available to
      // another task until it has also been given back three times.  Again it
      // is unlikely that real code would have these calls sequentially, but
      // instead buried in a more complex call structure.  This is just for
      // illustrative purposes.
      mutex.unlock();
      mutex.unlock();
      mutex.unlock();

      // Now the mutex can be taken by other tasks.
    } else {
      // We could not lock the mutex and can therefore not access the shared
      // resource safely.
    }
  }
}
