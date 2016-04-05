#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include "Assert.hpp"
#include "Task.hpp"

class Scheduler
{
    typedef std::lock_guard<std::mutex> ScopedLock;
    
    class Worker
    {
        bool running;
        int remainingJobs;
        TaskPtr task;
        std::thread thread;
        std::mutex stateLock;

        static void threadedRun(Worker* work);
        void run();

    public:
        Worker() : running(false), remainingJobs(0), task(nullptr) {}
        void start();
        void stop();

        void take(TaskPtr&& task);
        bool isIdle() const { return task == nullptr; }
        int getRemainingJobs() const { return remainingJobs; }
    };

    template<typename Input, typename Context>
    class TaskImpl : public Task
    {
        size_t idx;
        const Input* input;
        size_t size;
        const Context& context;

    public:
        TaskImpl(const Input* input, size_t size, const Context& context)
        : idx(0)
        , input(input)
        , size(size)
        , context(context)
        {}

        virtual void processNext()
        {
            ASSERT(!finished());
            context.process(input[idx]);
            ++idx;
        }

        virtual bool finished() const { return remaining() <= 0; }
        virtual size_t remaining() const { return size - idx; }
        
    };

    std::vector<Worker> workers;
    std::vector<Worker*> idleWorkers;
    std::vector<Worker*> activeWorkers;
    std::vector<TaskPtr> tasks;
    std::thread thread;
    std::mutex taskLock;
    int totalJobCount;
    bool active;

    static void doMonitorTasks(Scheduler* scheduler);
    void monitorTasks();

public:
    Scheduler(int numThreads);
    ~Scheduler();

    template<typename Input, typename Context>
    void schedule(const std::vector<Input>& in, const Context& context);

    template<typename Input, typename Context>
    void scheduleAsync(const std::vector<Input>& in, const Context& context);

    int getTotalJobCount() const { return totalJobCount; }
    bool isFinished() const { return tasks.empty() && activeWorkers.empty(); }
};

template<typename Input, typename Context>
void Scheduler::schedule(const std::vector<Input>& in, const Context& context)
{
    scheduleAsync<Input, Context>(in, context);

    while (!isFinished())
    {
        std::this_thread::yield();
    }
}


template<typename Input, typename Context>
void Scheduler::scheduleAsync(const std::vector<Input>& in, const Context& context)
{
    const auto taskCount = in.size();
    const auto batchCount = taskCount / workers.size();
    {
        ScopedLock lock(taskLock);
        for (int ii = 0; ii < taskCount; ii += batchCount)
        {
            tasks.emplace_back(new TaskImpl<Input, Context>(in.data() + ii, batchCount, context));
        }
        totalJobCount += taskCount;
    }
}
