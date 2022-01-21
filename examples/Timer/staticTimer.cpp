#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Timer.hpp>

constexpr size_t numberOfTimers = 5;

class MyTimer : public FreeRTOS::StaticTimer {
 public:
  MyTimer() : FreeRTOS::StaticTimer(0, true, "Timer") {}
  void timerFunction() final;

 private:
  uint8_t count = 0;
};

// Define a callback function that will be used by multiple timer instances. The
// callback function does nothing but count the number of times the associated
// timer expires, and stop the timer once the timer has expired 10 times.
void MyTimer::timerFunction() {
  constexpr uint8_t maxExpiryCountBeforeStopping = 10;

  // Optionally do something if the pxTimer parameter is NULL.
  configASSERT(!isValid());

  // Increment the count, then test to see if the timer has expired
  // maxExpiryCountBeforeStopping yet.
  count++;

  // If the timer has expired 10 times then stop it from running.
  if (count++ >= maxExpiryCountBeforeStopping) {
    // Do not use a block time if calling a timer API function from a timer
    // callback function, as doing so could cause a deadlock!
    stop();
  }
}

// An array of FreeRTOS::StaticTimer objects, which are used to store the state
// of each created timer.
static MyTimer xTimers[numberOfTimers];

int aFunction() {
  // Initialize then start some timers.  Starting the timers before the RTOS
  // scheduler has been started means the timers will start running immediately
  // that the RTOS scheduler starts.
  for (size_t x = 0; x < numberOfTimers; x++) {
    if (xTimers[x].isValid()) {
      xTimers[x].changePeriod((100 * x) + 100);
      if (!xTimers[x].start()) {
        // The timer could not be set into the Active state.
      }
    } else {
      // The timer was not created.
    }
  }

  // Create tasks here.

  // Starting the RTOS scheduler will start the timers running as they have
  // already been set into the active state.
  FreeRTOS::Kernel::startScheduler();

  // Should not reach here.
  for (;;) {
    ;
  }
}
