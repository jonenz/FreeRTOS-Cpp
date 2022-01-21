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
  FreeRTOS::EventGroup::EventBits bitsToClear;
  bitsToClear[0] = true;
  bitsToClear[4] = true;

  // Clear bit 0 and bit 4.
  auto bits = eventGroup.clear(bitsToClear);

  if (bits[0] && bits[4]) {
    //  Both bit 0 and bit 4 were set before clear() was called.  Both will now
    //  be clear (not set)
  } else if (bits[0]) {
    // Bit 0 was set before clear() was called.  It will now be clear.
  } else if (bits[4]) {
    // Bit 4 was set before clear() was called.  It will now be clear.
  } else {
    // Neither bit 0 nor bit 4 were set in the first place.
  }
}
