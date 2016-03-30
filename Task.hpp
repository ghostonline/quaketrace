#pragma once
#include <memory>

class Task
{
public:
    virtual ~Task() {}
    virtual void processNext() = 0;
    virtual bool finished() const = 0;
};

typedef std::unique_ptr<Task> TaskPtr;
