#pragma once

#include <vector>
#include <memory>

class Scheduler
{
    class Task
    {
    public:
        virtual ~Task() {}
        virtual bool processNext() = 0;
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

    typedef std::unique_ptr<Task> TaskPtr;

    static void doTask(Task* task);
    
    void doTasks(const std::vector<TaskPtr>& tasks) const;

public:
    template<typename Input, typename Output, typename Context>
    const std::vector<Output> schedule(const std::vector<Input>& in, const Context& context);
};

template<typename Input, typename Output, typename Context>
const std::vector<Output> Scheduler::schedule(const std::vector<Input>& in, const Context& context)
{

    static const int WORKER_COUNT = 4;
    std::vector<Output> out(in.size());
    const auto taskCount = in.size();
    const auto batchCount = taskCount / WORKER_COUNT;
    std::vector<TaskPtr> tasks;
    for (int ii = 0; ii < taskCount; ii += batchCount)
    {
        tasks.emplace_back(new TaskImpl<Input, Output, Context>(in.data() + ii, out.data() + ii, batchCount, context));
    }
    doTasks(tasks);
    return out;
}
