#include "Scheduler.hpp"
#include "Util.hpp"

static const std::chrono::seconds CONDITIONAL_WAIT_TIMEOUT(5);

void Scheduler::Worker::threadedRun(Worker* work) { work->run(); }

void Scheduler::Worker::run()
{
    UniqueLock lock(waitLock);
    while(running)
    {
        while (task == nullptr && running)
        {
            condition.wait_for(lock, CONDITIONAL_WAIT_TIMEOUT);
        }

        while (running && task && !task->finished())
        {
            task->processNext();
            remainingJobs = task->remaining();
            if (owner) { owner->wakeUp(); }
        }

        task = nullptr;
    }
}

void Scheduler::Worker::start(Scheduler* owner)
{
    ScopedLock lock(stateLock);
    ASSERT(!running);
    running = true;
    this->owner = owner;
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
    condition.notify_all();
}

Scheduler::Scheduler(int numThreads) : workers(numThreads), totalJobCount(0), active(true)
{
    for (int ii = numThreads - 1; ii >= 0; --ii)
    {
        auto& worker = workers[ii];
        worker.start(this);
        idleWorkers.push_back(&worker);
    }

    thread = std::thread(doMonitorTasks, this);
}

Scheduler::~Scheduler()
{
    {
        // Make sure the monitor thread is waiting to be notified
        ScopedLock lock(monitorLock);
        active = false;
    }
    monitorWait.notify_all();
    if (thread.joinable()) { thread.join(); }

    for (int ii = workers.size() - 1; ii >= 0; --ii)
    {
        workers[ii].stop();
    }
}

void Scheduler::monitorTasks()
{
    UniqueLock lock(monitorLock);
    while(active)
    {
        monitorWait.wait_for(lock, CONDITIONAL_WAIT_TIMEOUT);
        size_t jobs = 0;
        for (int ii = util::lastIndex(activeWorkers); ii >= 0; --ii)
        {
            auto worker = activeWorkers[ii];
            jobs += worker->getRemainingJobs();
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
                jobs += worker->getRemainingJobs();
            }

            for (int ii = util::lastIndex(tasks); ii >= 0; --ii)
            {
                jobs += tasks[ii]->remaining();
            }
        }

        totalJobCount = jobs;
    }
}
