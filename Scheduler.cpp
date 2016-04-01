#include "Scheduler.hpp"
#include "Util.hpp"

void Scheduler::Worker::threadedRun(Worker* work) { work->run(); }

void Scheduler::Worker::run()
{
    while(running)
    {
        while (task == nullptr && running)
        {
            std::this_thread::yield();
        }

        while (running && task && !task->finished())
        {
            task->processNext();
            std::this_thread::yield();
            remainingJobs = task->remaining();
        }

        task = nullptr;
    }
}

void Scheduler::Worker::start()
{
    ScopedLock lock(stateLock);
    ASSERT(!running);
    running = true;
    thread = std::thread(threadedRun, this);
}

void Scheduler::Worker::stop()
{
    ScopedLock lock(stateLock);
    ASSERT(running);
    running = false;
    thread.join();
}

void Scheduler::Worker::take(TaskPtr&& task)
{
    ScopedLock lock(stateLock);
    ASSERT(!this->task);
    this->task.swap(task);
    remainingJobs = this->task->remaining();
}

Scheduler::Scheduler(int numThreads) : workers(numThreads), totalJobCount(0), active(true)
{
    for (int ii = workers.size() - 1; ii >= 0; --ii)
    {
        auto& worker = workers[ii];
        worker.start();
        idleWorkers.push_back(&worker);
    }

    thread = std::thread(doMonitorTasks, this);
}

Scheduler::~Scheduler()
{
    active = false;
    if (thread.joinable()) { thread.join(); }

    for (int ii = workers.size() - 1; ii >= 0; --ii)
    {
        workers[ii].stop();
    }
}

void Scheduler::doMonitorTasks(Scheduler* scheduler)
{
    scheduler->monitorTasks();
}

void Scheduler::monitorTasks()
{
    while(active)
    {
        std::this_thread::yield();
        totalJobCount = 0;
        for (int ii = util::lastIndex(activeWorkers); ii >= 0; --ii)
        {
            auto worker = activeWorkers[ii];
            totalJobCount += worker->getRemainingJobs();
            if (worker->isIdle())
            {
                std::swap(activeWorkers[ii], activeWorkers.back());
                activeWorkers.pop_back();
                idleWorkers.push_back(worker);
            }
        }

        if (!tasks.empty() && !idleWorkers.empty())
        {
            ScopedLock lock(taskLock);
            while (!tasks.empty() && !idleWorkers.empty())
            {
                auto worker = idleWorkers.back();
                idleWorkers.pop_back();
                activeWorkers.push_back(worker);
                worker->take(std::move(tasks.back()));
                ASSERT(tasks.back() == nullptr);
                tasks.pop_back();
            }
        }
    }
}
