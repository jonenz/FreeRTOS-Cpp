#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  void sendMessage() {}
  void process_response_timeout() {}
  void process_response() {}
  void process_error() {}
};

MyTask myTask;

constexpr uint8_t messageReceivedBit = 4;
constexpr TickType_t ticksUntilTimeout = 100;

void MyTask::taskFunction() {
  for (;;) {
    // Send a message that expects a response.
    sendMessage();

    // Block this task until it has another pending notification. In this
    // example, the task only ever uses the messageReceivedBit of its
    // notification value, so the next event can only ever be on message
    // received.
    auto [received, notification] =
        notifyWait(0, 0, 0, ticksUntilTimeout);  // TODO: Clean up

    // If there wasn't a timeout, then the only possible event was received.  In
    // this example, that is the MESSAGE_RECEIVED_EVENT.
    if (notification.none()) {
      // Handle the response timeout.
      process_response_timeout();
    } else if (notification[messageReceivedBit]) {
      // Process the response event.
      process_response();
      notifyValueClear(1U << messageReceivedBit);
    } else {
      process_error();
    }
  }
}
