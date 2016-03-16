#include "Scheduler.hpp"
#include "Util.hpp"

void Scheduler::doTasks(const std::vector<TaskPtr>& tasks) const
{
    for (int ii = util::lastIndex(tasks); ii >= 0; --ii)
    {
        while(tasks[ii]->processNext()) {}
    }
}
