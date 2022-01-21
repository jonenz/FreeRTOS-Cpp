#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

// This is an example of a transmit function in a generic peripheral driver.  An
// RTOS task calls the transmit function, then waits in the Blocked state (so
// not using an CPU time) until it is notified that the transmission is
// complete. The transmission is performed by a DMA, and the DMA end interrupt
// is used to notify the task.

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask task;

FreeRTOS::TaskBase *taskToNotify = nullptr;

// The peripheral driver's transmit function.
void startTransmission(FreeRTOS::TaskBase *task, uint8_t *pcData,
                       size_t xDatalength) {
  // At this point taskToNotify should be nullptr as no transmission is in
  // progress.  A mutex can be used to guard access to the peripheral if
  // necessary.
  configASSERT(taskToNotify == nullptr);

  // Store the handle of the calling task.
  taskToNotify = task;

  // Start the transmission - an interrupt is generated when the transmission is
  // complete.
}

// The transmit end interrupt.
void transmitEndISR(void) {
  bool higherPriorityTaskWoken = false;

  // At this point taskToNotify should not be nullptr as a transmission was in
  // progress.
  configASSERT(taskToNotify != nullptr);

  // Notify the task that the transmission is complete.
  taskToNotify->notifyGiveFromISR(higherPriorityTaskWoken);

  if (higherPriorityTaskWoken) {
    /* If higherPriorityTaskWoken is true then a context switch should be
    performed to ensure the interrupt returns directly to the highest priority
    task.  The macro used for this purpose is dependent on the port in use and
    may be called portEND_SWITCHING_ISR(). */
    FreeRTOS::Kernel::yield();
  }

  // There are no transmissions in progress, so no tasks to notify.
  taskToNotify = nullptr;
}

void MyTask::taskFunction() {
  constexpr TickType_t xMaxBlockTime = pdMS_TO_TICKS(200);

  uint8_t data[8];

  startTransmission(this, data, sizeof(data));
}