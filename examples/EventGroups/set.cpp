#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  FreeRTOS::EventGroup eventGroup;
};

MyTask myTask;

void MyTask::taskFunction() {
  FreeRTOS::EventGroup::EventBits bitsToSet;
  bitsToSet[0] = true;
  bitsToSet[4] = true;

  // Set bit 0 and bit 4.
  auto bits = eventGroup.set(bitsToSet);

  if (bits[0] && bits[4]) {
    // Both bit 0 and bit 4 remained set when the function returned.
  } else if (bits[0]) {
    // Bit 0 remained set when the function returned, but bit 4 was cleared.  It
    // might be that bit 4 was cleared automatically as a task that was waiting
    // for bit 4 was removed from the Blocked state.
  } else if (bits[4]) {
    // Bit 4 remained set when the function returned, but bit 0 was cleared.  It
    // might be that bit 0 was cleared automatically as a task that was waiting
    // for bit 0 was removed from the Blocked state.
  } else {
    // Neither bit 0 nor bit 4 remained set.  It might be that a task was
    // waiting for both of the bits to be set, and the bits were cleared as the
    // task left the Blocked state.
  }
}
