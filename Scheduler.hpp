#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>

class Task
{
public:
    virtual ~Task() {}
    virtual bool processNext() = 0;
};

typedef std::unique_ptr<Task> TaskPtr;

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
        bool isDone();
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

        virtual bool processNext()
        {
            output[idx] = context.process(input[idx]);
            ++idx;
            return idx < size;
        }
    };

    std::vector<Worker> workers;
    std::vector<TaskPtr> tasks;

    void doWork();

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
    for (int ii = 0; ii < taskCount; ii += batchCount)
    {
        tasks.emplace_back(new TaskImpl<Input, Output, Context>(in.data() + ii, out.data() + ii, batchCount, context));
    }
    doWork();
    return out;
}
