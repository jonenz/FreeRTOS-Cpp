#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void vAFunction() {
  // Create a task before starting the kernel.
  MyTask task;

  // Start the real time kernel with preemption.
  FreeRTOS::Kernel::startScheduler();

  // Will not get here unless a task calls FreeRTOS::Kernel::endScheduler()
}
