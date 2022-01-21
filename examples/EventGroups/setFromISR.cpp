#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>

FreeRTOS::StaticEventGroup eventGroup;

void anInterruptHandler() {
  // higherPriorityTaskWoken must be initialised to false.
  bool higherPriorityTaskWoken = false;

  // Set bit 0 and bit 4 in eventGroup.
  auto result = eventGroup.setFromISR(higherPriorityTaskWoken, 0b1001);

  // Was the message posted successfully?
  if (result) {
  }

  if (higherPriorityTaskWoken) {
    // If higherPriorityTaskWoken is now set to true then a context switch
    // should be requested.
    FreeRTOS::Kernel::yield();
  }
}
