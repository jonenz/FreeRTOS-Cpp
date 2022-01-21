#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  void demoFunction();
};

void MyTask::demoFunction() {
  // Enter the critical section.  In this example, this function is itself
  // called from within a critical section, so entering this critical section
  // will result in a nesting depth of 2.
  FreeRTOS::Kernel::enterCritical();

  // Perform the action that is being protected by the critical section here.

  // Exit the critical section.  In this example, this function is itself called
  // from a critical section, so this call to FreeRTOS::Kernel::exitCritical()
  // will decrement the nesting count by one, but not result in interrupts
  // becoming enabled.
  FreeRTOS::Kernel::exitCritical();
}

void MyTask::taskFunction() {
  for (;;) {
    // Perform some functionality here.

    // Call FreeRTOS::Kernel::enterCritical() to create a critical section.
    FreeRTOS::Kernel::enterCritical();

    // Execute the code that requires the critical section here.

    // Calls to FreeRTOS::Kernel::enterCritical() can be nested so it is safe to
    // call a function that includes its own calls to
    // FreeRTOS::Kernel::enterCritical() and FreeRTOS::Kernel::exitCritical().
    demoFunction();

    // The operation that required the critical section is complete so exit the
    // critical section.  After this call to FreeRTOS::Kernel::exitCritical(),
    // the nesting depth will be zero, so interrupts will have been re-enabled.
    FreeRTOS::Kernel::exitCritical();
  }
}
