#include <FreeRTOS/Timer.hpp>

class MyTimer : public FreeRTOS::StaticTimer {
 public:
  MyTimer() : FreeRTOS::StaticTimer(pdMS_TO_TICKS(500)) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  TickType_t timerPeriod;

  // Query the period of the timer that expires.
  timerPeriod = getPeriod();
}

static MyTimer myTimer;
