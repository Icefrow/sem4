#include "pch.h"
#include "RefCountedObject.h"

RefCountedObject::~RefCountedObject()
{
}

void intrusive_ptr_release(const RefCountedObject * obj) throw()
{
    if (--obj->refCount_)
        return;

    try {
        delete obj;
    }
    catch (...) {}
}
