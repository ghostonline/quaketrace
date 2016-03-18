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

bool Scheduler::Worker::isDone() const
{
    return task->finished();
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

void Scheduler::doWork()
{
    for (int ii = util::lastIndex(workers); ii >= 0 ; --ii)
    {
        workers[ii].doTask(tasks[ii].get());
    }

    bool unfinished = true;
    while (unfinished)
    {
        unfinished = false;
        for (int ii = util::lastIndex(workers); ii >= 0; --ii)
        {
            Worker& worker = workers[ii];
            if (!worker.isDone())
            {
                unfinished = true;
                break;
            }
        }

        std::this_thread::yield();
    }
    
    tasks.clear();
}
