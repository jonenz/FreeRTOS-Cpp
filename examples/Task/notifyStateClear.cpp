#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final {}

 private:
  void serialPutString(const signed char* const pcStringToSend,
                       uint16_t usStringLength);
};

// Fake peripherial interface functions.
void UARTSendString(const signed char* const pcStringToSend,
                    uint16_t usStringLength) {}

MyTask myTask;

// An example UART send function.  The function starts a UART transmission then
// waits to be notified that the transmission is complete.  The transmission
// complete notification is sent from the UART interrupt.  The calling task's
// notification state is cleared before the transmission is started to ensure it
// is not co-incidentally already pending before the task attempts to block on
// its notification state.
void MyTask::serialPutString(const signed char* const pcStringToSend,
                             uint16_t usStringLength) {
  constexpr TickType_t maxBlockTime = pdMS_TO_TICKS(5000);

  // sendingTask holds the handle of the task waiting for the transmission to
  // complete.  If sendingTask is nullptr then a transmission is not in
  // progress.  Don't start to send a new string unless transmission of the
  // previous string is complete.
  if (usStringLength > 0) {
    // Ensure the task's 0th notification state is not already pending.
    notifyStateClear();

    // Start sending the string - the transmission is then controlled by an
    // interrupt.
    UARTSendString(pcStringToSend, usStringLength);

    // Wait in the Blocked state (so not using any CPU time) until the UART ISR
    // sends the 0th notification to sendingTask to notify (and unblock) the
    // task when the transmission is complete.
    notifyTake();
  }
}
