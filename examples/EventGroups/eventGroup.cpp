#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;

 private:
  FreeRTOS::EventGroup eventGroup;
};

MyTask myTask;

void MyTask::taskFunction() {
  // Was the event group created successfully?
  if (eventGroup.isValid()) {
    // The event group was created.
  } else {
    // The event group was not created because there was insufficient FreeRTOS
    // heap available.
  }
}
