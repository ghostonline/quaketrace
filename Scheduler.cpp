#include "Scheduler.hpp"
#include "Util.hpp"
#include <thread>

void Scheduler::doTask(Task* task)
{
    while(task->processNext()) {}
}

void Scheduler::doTasks(const std::vector<TaskPtr>& tasks) const
{
    std::vector<std::thread> threads;
    for (int ii = util::lastIndex(tasks); ii >= 0; --ii)
    {
        threads.push_back(std::thread(Scheduler::doTask, tasks[ii].get()));
    }

    for (int ii = util::lastIndex(threads); ii >= 0; --ii)
    {
        ASSERT(threads[ii].joinable());
        threads[ii].join();
    }
}
