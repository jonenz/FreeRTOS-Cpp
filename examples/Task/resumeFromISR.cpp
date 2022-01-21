#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask myTask;

void MyTask::taskFunction() {
  // The task being suspended and resumed.
  for (;;) {
    // ... Perform some function here.

    // The task suspends itself.
    suspend();

    // The task is now suspended, so will not reach here until the ISR resumes
    // it.
  }
}

void anExampleISR() {
  // Resume the suspended task.
  if (myTask.resumeFromISR()) {
    // We should switch context so the ISR returns to a different task.
    // NOTE:  How this is done depends on the port you are using.  Check the
    // documentation and examples for your port.
    FreeRTOS::Kernel::yield();
  }
}
