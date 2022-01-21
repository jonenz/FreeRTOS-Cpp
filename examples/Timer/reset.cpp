#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Timer.hpp>

// Fake backlight interface functions.
constexpr bool backlightOff = false;
constexpr bool backlightOn = true;
void vSetBacklightState(bool);

// When a key is pressed, an LCD back-light is switched on.  If 5 seconds pass
// without a key being pressed, then the LCD back-light is switched off.  In
// this case, the timer is a one-shot timer.
class BacklightTimer : public FreeRTOS::StaticTimer {
 public:
  BacklightTimer()
      : FreeRTOS::StaticTimer(pdMS_TO_TICKS(5000), false, "BacklightTimer") {}
  void timerFunction() final;
};

void BacklightTimer::timerFunction() {
  // The timer expired, therefore 5 seconds must have passed since a key was
  // pressed.  Switch off the LCD back-light.
  vSetBacklightState(backlightOff);
}

static BacklightTimer backlightTimer;

// The key press event handler.
void keyPressEventHandler() {
  // Ensure the LCD back-light is on, then restart the timer that is responsible
  // for turning the back-light off after 5 seconds of key inactivity.  Wait 10
  // ticks for the command to be successfully sent if it cannot be sent
  // immediately.
  vSetBacklightState(backlightOn);

  if (!backlightTimer.reset(10)) {
    // The reset command was not executed successfully.  Take appropriate action
    // here.
  }

  // Perform the rest of the key processing here.
}

int main() {
  if (!backlightTimer.isValid()) {
    // The timer was not created.
  } else {
    // Start the timer.  No block time is specified, and even if one was it
    // would be ignored because the RTOS scheduler has not yet been started.
    if (!backlightTimer.start()) {
      // The timer could not be set into the active state.
    }
  }

  // Create tasks here.

  // Starting the RTOS scheduler will start the timer running as it has already
  // been set into the active state.
  FreeRTOS::Kernel::startScheduler();

  /* Should not reach here. */
  for (;;) {
    ;
  }
}
