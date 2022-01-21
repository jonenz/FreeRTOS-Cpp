#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  for (;;) {
    // Task code goes here.

    // At some point we want to end the real time kernel processing so call ...
    FreeRTOS::Kernel::endScheduler();
  }
}

void vAFunction() {
  // Create at least one task before starting the RTOS kernel.
  MyTask task;

  // Start the real time kernel with preemption.
  FreeRTOS::Kernel::startScheduler();

  // Will only get here when MyTask::taskFunction() task has called
  // FreeRTOS::Kernel::endScheduler().  When we get here we are back to single
  // task execution.
}
