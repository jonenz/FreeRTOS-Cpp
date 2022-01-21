#include <FreeRTOS/Timer.hpp>

class MyTimer : public FreeRTOS::Timer {
 public:
  MyTimer() : FreeRTOS::Timer(0) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  // Timer function.
}

void aFunction() {
  MyTimer timer;

  if (timer.isActive()) {
    // Timer will be deleted when the object goes out of scope.
    return;
  }

  // timer is not active, change its period to 500ms.  This will also cause the
  // timer to start.  Block for a maximum of 100 ticks if the change period
  // command cannot immediately be sent to the timer command queue.
  if (timer.changePeriod((500 / portTICK_PERIOD_MS), 100)) {
    // The command was successfully sent.
  } else {
    //  The command could not be sent, even after waiting for 100 ticks to pass.
    //  Take appropriate action here.
  }
}
