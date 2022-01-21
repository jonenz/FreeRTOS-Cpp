#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>

FreeRTOS::StaticEventGroup eventGroup;

void anInterruptHandler() {
  // Set bit 0 and bit 4 in eventGroup.
  if (eventGroup.clearFromISR(0b1001)) {
    // The command was sent to the daemon task.
  } else {
    // The clear bits command was not sent to the daemon task.
  }
}
