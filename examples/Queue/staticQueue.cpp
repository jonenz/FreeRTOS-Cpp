#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  // The queue is to be created to hold a maximum of 10 uint64_t variables.
  static FreeRTOS::StaticQueue<uint64_t, 10> queue;

  // Queue should be valid
  configASSERT(queue.isValid());
}
