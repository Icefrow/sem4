#pragma once

class NonCopyable
{
protected:
    NonCopyable() {}
public:
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable & operator=(NonCopyable &&) = delete;
};

