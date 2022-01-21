#include <FreeRTOS/Timer.hpp>

class MyTimer : public FreeRTOS::Timer {
 public:
  MyTimer() : FreeRTOS::Timer(100) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  // Timer function.
}

void aFunction() {
  MyTimer timer;

  if (timer.isActive()) {
    // Timer is active, do something.
  } else {
    // Timer is not active, do something else.
  }
}
