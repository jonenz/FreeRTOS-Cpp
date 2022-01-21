#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

void MyTask::taskFunction() {
  // Perform an action every 10 ticks.
  constexpr TickType_t xFrequency = 10;

  for (;;) {
    // Wait for the next cycle.
    auto wasDelayed = delayUntil(xFrequency);

    // Perform action here. wasDelayed value can be used to determine
    // whether a deadline was missed if the code here took too long.
    if (wasDelayed) {
      // Check for deadline miss.
    }
  }
}
