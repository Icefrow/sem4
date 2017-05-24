#pragma once

#include <memory>

#include "NonCopyable.h"

template<typename T>
class Buffer : NonCopyable
{
public:
    explicit Buffer(size_t size)
        : mem_(new T[size])
    {}

    void swap(Buffer & other) noexcept {
        mem_.swap(other.mem_);
    }

    T * get() {
        return mem_.get();
    }

    const T * get() const {
        return mem_.get();
    }

    T * detach() {
        return mem_.release();
    }

private:
    std::unique_ptr<T[]> mem_;
};

