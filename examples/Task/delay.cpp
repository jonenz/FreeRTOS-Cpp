#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  void toggleLED() {}
};

void MyTask::taskFunction() {
  // Block for 500ms.
  constexpr TickType_t xDelay = 500 / portTICK_PERIOD_MS;

  for (;;) {
    // Simply toggle the LED every 500ms, blocking between each toggle.
    toggleLED();
    delay(xDelay);
  }
}
