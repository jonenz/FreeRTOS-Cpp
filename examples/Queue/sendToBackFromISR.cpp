#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Queue.hpp>

// Fake interface to get data from.
char getByte() { return 'A'; }
size_t getBytesRemaining() { return 0; }

// Queue that holds 10 characters.
FreeRTOS::StaticQueue<char, 10> rxQueue;

void bufferISR() {
  // We have not woken a task at the start of the ISR.
  bool higherPriorityTaskWoken = false;

  // Loop until the buffer is empty.
  do {
    // Obtain a byte from the buffer and post the byte.
    rxQueue.sendToBackFromISR(higherPriorityTaskWoken, getByte());

  } while (getBytesRemaining() > 0);

  // Now the buffer is empty we can switch context if necessary.
  if (higherPriorityTaskWoken) {
    FreeRTOS::Kernel::yield();
  }
}
