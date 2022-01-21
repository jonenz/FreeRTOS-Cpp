#include <FreeRTOS/Timer.hpp>

// Fake backlight interface functions.
constexpr bool backlightOff = false;
constexpr bool backlightOn = true;
void vSetBacklightState(bool);

// When a key is pressed, an LCD back-light is switched on.  If 5 seconds pass
// without a key being pressed, then the LCD back-light is switched off.  In
// this case, the timer is a one-shot timer, and unlike the example given for
// the reset() function, the key press event handler is an interrupt service
// routine.
class BacklightTimer : public FreeRTOS::StaticTimer {
 public:
  BacklightTimer() : FreeRTOS::StaticTimer(pdMS_TO_TICKS(5000)) {}
  void timerFunction() final;
};

void BacklightTimer::timerFunction() {
  // The timer expired, therefore 5 seconds must have passed since a key was
  // pressed.  Switch off the LCD back-light.
  vSetBacklightState(backlightOff);
}

static BacklightTimer backlightTimer;

// The key press interrupt service routine.
void keyPressEventInterruptHandler() {
  bool higherPriorityTaskWoken = false;

  // Ensure the LCD back-light is on, then restart the timer that is responsible
  // for turning the back-light off after 5 seconds of key inactivity.  This is
  // an interrupt service routine so can only call FreeRTOS API functions that
  // end in "FromISR".
  vSetBacklightState(backlightOn);

  // startFromISR() or resetFromISR() could be called here as both cause the
  // timer to re-calculate its expiry time. higherPriorityTaskWoken was
  // initialised to false when it was declared (in this function).
  if (!backlightTimer.resetFromISR(higherPriorityTaskWoken)) {
    // The reset command was not executed successfully.  Take appropriate action
    // here.
  }

  // Perform the rest of the key processing here.

  // If higherPriorityTaskWoken equals true, then a context switch should be
  // performed.  The syntax required to perform a context switch from inside an
  // ISR varies from port to port, and from compiler to compiler.  Inspect the
  // demos for the port you are using to find the actual syntax required.
  if (higherPriorityTaskWoken) {
    // Call the interrupt safe yield function here (actual function depends on
    // the FreeRTOS port being used).
  }
}
