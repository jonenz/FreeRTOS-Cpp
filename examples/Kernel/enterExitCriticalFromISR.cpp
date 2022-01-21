#include <FreeRTOS/Kernel.hpp>

void demoFunction() {
  uint32_t savedInterruptStatus;

  // Enter the critical section.  In this example, this function is itself
  // called from within a critical section, so entering this critical section
  // will result in a nesting depth of 2. Save the value returned by
  // FreeRTOS::Kernel::enterCriticalFromISR() into a local stack variable so it
  // can be passed into FreeRTOS::Kernel::exitCriticalFromISR().
  savedInterruptStatus = FreeRTOS::Kernel::enterCriticalFromISR();

  // Perform the action that is being protected by the critical section here.

  // Exit the critical section.  In this example, this function is itself called
  // from a critical section, so interrupts will have already been disabled
  // before a value was stored in savedInterruptStatus, and therefore passing
  // savedInterruptStatus into FreeRTOS::Kernel::exitCriticalFromISR() will not
  // result in interrupts being re-enabled.
  FreeRTOS::Kernel::exitCriticalFromISR(savedInterruptStatus);
}

void demoISR() {
  uint32_t savedInterruptStatus;

  // Call FreeRTOS::Kernel::enterCriticalFromISR() to create a critical section,
  // saving the returned value into a local stack variable.
  savedInterruptStatus = FreeRTOS::Kernel::enterCriticalFromISR();

  // Execute the code that requires the critical section here.

  // Calls to FreeRTOS::Kernel::enterCriticalFromISR() can be nested so it is
  // safe to call a function that includes its own calls to
  // FreeRTOS::Kernel::enterCriticalFromISR() and
  // FreeRTOS::Kernel::exitCriticalFromISR().
  demoFunction();

  // The operation that required the critical section is complete so exit the
  // critical section.  Assuming interrupts were enabled on entry to this ISR,
  // the value saved in savedInterruptStatus will result in interrupts being
  // re-enabled.
  FreeRTOS::Kernel::exitCriticalFromISR(savedInterruptStatus);
}
