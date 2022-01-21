#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Queue.hpp>

constexpr char emergencyMessage = 'E';

// Fake interface to get data from.
char getByte() { return 'A'; }
size_t getBytesRemaining() { return 0; }

// Queue that holds 10 characters.
FreeRTOS::StaticQueue<char, 10> rxQueue;

void bufferISR() {
  // We have not woken a task at the start of the ISR.
  bool higherPriorityTaskWoken = false;

  // Obtain a byte from the buffer.
  char cIn = getByte();

  if (cIn == emergencyMessage) {
    // Post the byte to the front of the queue.
    rxQueue.sendToFrontFromISR(higherPriorityTaskWoken, cIn);
  } else {
    // Post the byte to the back of the queue.
    rxQueue.sendToBackFromISR(higherPriorityTaskWoken, cIn);
  }

  // Did sending to the queue unblock a higher priority task?
  if (higherPriorityTaskWoken) {
    FreeRTOS::Kernel::yield();
  }
}
