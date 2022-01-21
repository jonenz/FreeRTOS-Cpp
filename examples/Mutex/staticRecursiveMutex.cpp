#include <FreeRTOS/Mutex.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

static FreeRTOS::StaticRecursiveMutex mutex;

void MyTask::taskFunction() {
  // Rest of the task code goes here.
}
