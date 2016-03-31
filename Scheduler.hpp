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
        TaskPtr task;
        std::thread thread;
        std::mutex stateLock;

        static void threadedRun(Worker* work);
        void run();

    public:
        Worker() : running(false), task(nullptr) {}
        void start();
        void stop();

        void take(TaskPtr&& task);
        bool isIdle() const { return task == nullptr; }
    };

    template<typename Input, typename Output, typename Context>
    class TaskImpl : public Task
    {
        size_t idx;
        const Input* input;
        Output* output;
        size_t size;
        const Context& context;

    public:
        TaskImpl(const Input* input, Output* output, size_t size, const Context& context)
        : idx(0)
        , input(input)
        , output(output)
        , size(size)
        , context(context)
        {}

        virtual void processNext()
        {
            ASSERT(!finished());
            output[idx] = context.process(input[idx]);
            ++idx;
        }

        virtual bool finished() const { return idx >= size; }

    };

    std::vector<Worker> workers;
    std::vector<Worker*> idleWorkers;
    std::vector<Worker*> activeWorkers;
    std::vector<TaskPtr> tasks;
    std::thread thread;
    std::mutex taskLock;
    bool active;

    bool isFinished() const { return tasks.empty() && activeWorkers.empty(); }
    static void doMonitorTasks(Scheduler* scheduler);
    void monitorTasks();

public:
    Scheduler(int numThreads);
    ~Scheduler();

    template<typename Input, typename Output, typename Context>
    const std::vector<Output> schedule(const std::vector<Input>& in, const Context& context);
};

template<typename Input, typename Output, typename Context>
const std::vector<Output> Scheduler::schedule(const std::vector<Input>& in, const Context& context)
{
    std::vector<Output> out(in.size());
    const auto taskCount = in.size();
    const auto batchCount = taskCount / workers.size();
    {
        ScopedLock lock(taskLock);
        for (int ii = 0; ii < taskCount; ii += batchCount)
        {
            tasks.emplace_back(new TaskImpl<Input, Output, Context>(in.data() + ii, out.data() + ii, batchCount, context));
        }
    }

    while (!isFinished())
    {
        std::this_thread::yield();
    }

    return out;
}
