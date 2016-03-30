#include "Scheduler.hpp"
#include "Util.hpp"
#include <chrono>

typedef std::lock_guard<std::mutex> ScopedLock;

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

void Scheduler::Worker::doTask(Task* task)
{
    ScopedLock lock(stateLock);
    ASSERT(!this->task);
    this->task = task;
}

Scheduler::Scheduler(int numThreads) : workers(numThreads)
{
    for (int ii = workers.size() - 1; ii >= 0; --ii)
    {
        workers[ii].start();
    }
}

Scheduler::~Scheduler()
{
    for (int ii = workers.size() - 1; ii >= 0; --ii)
    {
        workers[ii].stop();
    }
}

void Scheduler::startWork(const std::vector<TaskPtr>& tasks)
{
    for (int ii = util::lastIndex(workers); ii >= 0 ; --ii)
    {
        workers[ii].doTask(tasks[ii].get());
    }
}

bool Scheduler::isFinished() const
{
    for (int ii = util::lastIndex(workers); ii >= 0; --ii)
    {
        const Worker& worker = workers[ii];
        if (!worker.isIdle())
        {
            return false;
        }
    }

    return true;
}
