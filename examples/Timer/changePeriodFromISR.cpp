#include <FreeRTOS/Timer.hpp>

class MyTimer : public FreeRTOS::StaticTimer {
 public:
  MyTimer() : FreeRTOS::StaticTimer(pdMS_TO_TICKS(5000)) {}
  void timerFunction() final;
};

void MyTimer::timerFunction() {
  // Timer function.
}

static MyTimer myTimer;

// The interrupt service routine that changes the period of timer.
void anExampleInterruptServiceRoutine() {
  bool higherPriorityTaskWoken = false;

  // The interrupt has occurred - change the period of xTimer to 500ms.
  // higherPriorityTaskWoken was set to false where it was defined (within this
  // function).  As this is an interrupt service routine, only FreeRTOS API
  // functions that end in "FromISR" can be used.
  if (!myTimer.changePeriodFromISR(higherPriorityTaskWoken,
                                   pdMS_TO_TICKS(500))) {
    // The command to change the timers period was not executed successfully.
    // Take appropriate action here.
  }

  // If higherPriorityTaskWoken equals true, then a context switch should be
  // performed.  The syntax required to perform a context switch from inside an
  // ISR varies from port to port, and from compiler to compiler.  Inspect the
  // demos for the port you are using to find the actual syntax required.
  if (higherPriorityTaskWoken) {
    // Call the interrupt safe yield function here (actual function depends on
    // the FreeRTOS port being used).
  }
}
