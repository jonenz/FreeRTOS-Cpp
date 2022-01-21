#include <FreeRTOS/EventGroups.hpp>
#include <FreeRTOS/Kernel.hpp>
#include <FreeRTOS/Task.hpp>

class MyTask : public FreeRTOS::Task {
 public:
  void taskFunction() final;
};

MyTask myTask;

FreeRTOS::StaticEventGroup eventGroup;

void MyTask::taskFunction() {
  // Use the event group
}
