#pragma once

#include <vector>
#include "Util.hpp"

class Scheduler
{
public:
    template<typename Input, typename Output>
    const std::vector<Output> schedule(std::vector<Input> in);

    template<typename Input, typename Output>
    const Output process(Input in);
};

template<typename Input, typename Output>
const std::vector<Output> Scheduler::schedule(std::vector<Input> in)
{
    std::vector<Output> out(in.size());
    for (int ii = in.size() - 1; ii >= 0; --ii)
    {
        out[ii] = process<Input, Output>(in[ii]);
    }
    return out;
}
