#include <FreeRTOS/Mutex.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

static FreeRTOS::StaticMutex mutex;

void MyTask::taskFunction() {
  // Rest of the task code goes here.
}
