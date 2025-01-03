#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

class AnalysisTaskManager {
public:
  ~AnalysisTaskManager() {
    if (currentTask.valid()) {
      currentTask.wait();
    }
  }

  void submitTask(std::function<void()> task) {
    if (currentTask.valid()) {
      std::cout << "Cancelling previous task...\n";
    }
    currentTask = std::async(std::launch::async, std::move(task));
  }

private:
  std::future<void> currentTask;
};

void expensiveComputation() {
  for (int i = 0; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Processing step " << i + 1 << "\n";
  }
  std::cout << "Task completed.\n";
}

int main() {
  AnalysisTaskManager manager;

  manager.submitTask(expensiveComputation);
  std::this_thread::sleep_for(std::chrono::milliseconds(250));
  manager.submitTask(expensiveComputation);
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));

  return 0;
}
