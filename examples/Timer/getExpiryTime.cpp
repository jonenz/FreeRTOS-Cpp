#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Timer.hpp>

class MyTimer : public FreeRTOS::StaticTimer {
 public:
  MyTimer() : FreeRTOS::StaticTimer(pdMS_TO_TICKS(500)) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  TickType_t remainingTime;

  // Query the period of the timer that expires.
  remainingTime = getExpiryTime() - FreeRTOS::Kernel::getTickCount();
}

static MyTimer myTimer;
