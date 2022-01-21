#include <FreeRTOS/Semaphore.hpp>
#include <FreeRTOS/Task.hpp>

#define LONG_TIME 0xffff
#define TICKS_TO_WAIT 10

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

static FreeRTOS::StaticBinarySemaphore binarySemaphore;

void MyTask::taskFunction() {
  // We are using the semaphore for synchronisation so we create a binary
  // semaphore rather than a mutex.  We must make sure that the interrupt does
  // not attempt to use the semaphore before it is created!

  for (;;) {
    // We want this task to run every 10 ticks of a timer.  The semaphore was
    // created before this task was started.

    // Block waiting for the semaphore to become available.
    if (binarySemaphore.take()) {
      // It is time to execute.

      // ...

      // We have finished our task.  Return to the top of the loop where we will
      // block on the semaphore until it is time to execute again.  Note when
      // using the semaphore for synchronisation with an ISR in this manner
      // there is no need to 'give' the semaphore back.
    }
  }
}

// Timer ISR
void vTimerISR(void* pvParameters) {
  static TickType_t ucLocalTickCount = 0;
  bool higherPriorityTaskWoken = false;

  // A timer tick has occurred.

  // ... Do other time functions.

  // Is it time for MyTask() to run?
  ucLocalTickCount++;
  if (ucLocalTickCount >= TICKS_TO_WAIT) {
    // Unblock the task by releasing the semaphore.
    binarySemaphore.giveFromISR(higherPriorityTaskWoken);

    // Reset the count so we release the semaphore again in 10 ticks time.
    ucLocalTickCount = 0;
  }

  // If higherPriorityTaskWoken was set to true you we should yield.  The actual
  // macro used here is port specific.
  FreeRTOS::Kernel::yield();
}
