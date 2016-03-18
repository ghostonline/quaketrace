#include "Scheduler.hpp"
#include "Util.hpp"
#include <chrono>

typedef std::lock_guard<std::mutex> ScopedLock;

void Scheduler::Worker::threadedRun(Worker* work) { work->run(); }

void Scheduler::Worker::run()
{
    while(running)
    {
        static const std::chrono::milliseconds timeout(100);
        while (task == nullptr && running) { std::this_thread::sleep_for(timeout); }

        while (running && task && task->processNext()) {}
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

bool Scheduler::Worker::isDone()
{
    return !task;
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

        static const std::chrono::microseconds SLEEP_TIME(100);
        std::this_thread::sleep_for(SLEEP_TIME);
    }
    
    tasks.clear();
}
