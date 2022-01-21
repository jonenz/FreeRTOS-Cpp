#include <FreeRTOS/Mutex.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  FreeRTOS::Mutex mutex;

  if (mutex.isValid()) {
    // The recursive mutex was created successfully and can now be used.
  }
}
