#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Queue.hpp>
#include <FreeRTOS/Task.hpp>

// Fake interface to send data to.
void sendCharacter(char c) {}

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

// Queue that holds 10 characters.
FreeRTOS::Queue<char> queue(10);

void MyTask::taskFunction() {
  constexpr TickType_t ticksToWait = 0xff;

  if (!queue.isValid()) {
    // Failed to create the queue.
  }

  // Post some characters that will be used within an ISR.  If the queue is full
  // then this task will block for ticksToWait ticks.
  queue.sendToBack('a', ticksToWait);
  queue.sendToBack('b', ticksToWait);

  // ... keep posting characters ... this task may block when the queue becomes
  // full.

  queue.sendToBack('c', ticksToWait);
}

// ISR that outputs all the characters received on the queue.
void ISR_Routine() {
  bool taskWokenByReceive = false;

  while (auto character = queue.receiveFromISR(taskWokenByReceive)) {
    // A character was received.  Output the character now.
    sendCharacter(character.value());
  }

  // If removing the character from the queue woke the task that was posting
  // onto the queue taskWokenByReceive will have been set to true.  No matter
  // how many times this loop iterates only one task will be woken.
  if (taskWokenByReceive) {
    // We should switch context so the ISR returns to a different task.
    // NOTE:  How this is done depends on the port you are using.  Check the
    // documentation and examples for your port.
    FreeRTOS::Kernel::yield();
  }
}
