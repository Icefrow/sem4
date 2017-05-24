#pragma once
#include <util/NonCopyable.h>
#include <atomic>

class RefCountedObject : NonCopyable
{
protected:
    explicit RefCountedObject(size_t initialRefCount = 0)
        : refCount_(initialRefCount)
    {}

    virtual ~RefCountedObject(void);

    size_t refCount() const {
        return refCount_;
    }

private:
    volatile mutable std::atomic_size_t refCount_;

    friend void intrusive_ptr_add_ref(const RefCountedObject *);
    friend void intrusive_ptr_release(const RefCountedObject *) throw();
};

// increment will be inline
inline void intrusive_ptr_add_ref(const RefCountedObject * obj)
{
    ++obj->refCount_;
}
