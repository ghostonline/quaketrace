#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include "Assert.hpp"
#include "Task.hpp"

class Scheduler
{
    class Worker
    {
        bool running;
        Task* task;
        std::thread thread;
        std::mutex stateLock;

        static void threadedRun(Worker* work);
        void run();

    public:
        Worker() : running(false), task(nullptr) {}
        void start();
        void stop();

        void doTask(Task* task);
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

    void startWork(const std::vector<TaskPtr>& tasks);
    bool isFinished() const;

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
    std::vector<TaskPtr> tasks;
    for (int ii = 0; ii < taskCount; ii += batchCount)
    {
        tasks.emplace_back(new TaskImpl<Input, Output, Context>(in.data() + ii, out.data() + ii, batchCount, context));
    }

    startWork(tasks);

    while (!isFinished())
    {
        std::this_thread::yield();
    }

    tasks.clear();

    return out;
}
