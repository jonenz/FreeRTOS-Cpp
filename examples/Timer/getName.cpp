#include <FreeRTOS/Timer.hpp>

#include "cstring"

constexpr char const *pcTimerName = "ExampleTimer";

class MyTimer : public FreeRTOS::Timer {
 public:
  MyTimer() : FreeRTOS::Timer(pdMS_TO_TICKS(500), true, pcTimerName) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  // Timer function.
}

int main() {
  MyTimer myTimer;

  if (myTimer.isValid()) {
    myTimer.start(portMAX_DELAY);

    // Just to demonstrate getName(), query the timer's name and assert if it
    // does not equal pcTimerName.
    configASSERT(strcmp(myTimer.getName(), pcTimerName) == 0);
  }
}
