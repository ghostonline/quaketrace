#pragma once

#include <vector>

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
    
public:
    template<typename Input, typename Output, typename Context>
    const std::vector<Output> schedule(const std::vector<Input>& in, const Context& context);
};

template<typename Input, typename Output, typename Context>
const std::vector<Output> Scheduler::schedule(const std::vector<Input>& in, const Context& context)
{
    std::vector<Output> out(in.size());
    std::unique_ptr<Task> task(new TaskImpl<Input, Output, Context>(in.data(), out.data(), in.size(), context));
    while(task->processNext()) {}
    return out;
}
