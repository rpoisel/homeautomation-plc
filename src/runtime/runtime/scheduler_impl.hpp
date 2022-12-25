#pragma once

#include <runtime.hpp>
#include <scheduler.hpp>

#include <list>
#include <tuple>
#include <utility>

namespace HomeAutomation {
namespace Runtime {

class TaskImpl : public Task {
public:
  TaskImpl(HomeAutomation::Scheduler::Task &task) : task{task} {}
  virtual ~TaskImpl() = default;

  void addProgram(
      std::shared_ptr<HomeAutomation::Scheduler::Program> program) override {
    task.programs.push_back(program);
  }

private:
  HomeAutomation::Scheduler::Task &task;
};

class SchedulerImpl final : public Scheduler {
public:
  SchedulerImpl() = default;

  void
  installTask(std::string const &name,
              std::shared_ptr<HomeAutomation::Scheduler::TaskIOLogicComposite>
                  taskLogic,
              HomeAutomation::Scheduler::milliseconds interval) {
    scheduler.installTask(name, taskLogic, interval);
    auto task = scheduler.getTask(name);
    tasks.emplace(std::piecewise_construct, std::forward_as_tuple(name),
                  std::forward_as_tuple(*task));
  }

  Task *getTask(std::string const &name) override {
    auto const &it = tasks.find(name);
    if (it == tasks.end()) {
      return nullptr;
    }
    return &it->second;
  }

  void start(HomeAutomation::Scheduler::QuitCb quitCb) {
    scheduler.start(quitCb);
  }

  int wait() { return scheduler.wait(); }

private:
  HomeAutomation::Scheduler::Scheduler scheduler;
  std::map<std::string, TaskImpl> tasks;
};

} // namespace Runtime
} // namespace HomeAutomation
