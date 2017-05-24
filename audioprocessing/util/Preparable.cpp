#include "pch.h"
#include <util/Preparable.h>

namespace {
Preparable * s_first = nullptr;
}

Preparable::Preparable()
{
    next_ = s_first;
    prev_ = &s_first;
    s_first = this;

    if(next_)
        next_->prev_ = &next_;
}

Preparable::~Preparable()
{
    *prev_ = next_;
}

void Preparable::prepareAll()
{
    Preparable * walker = s_first;

    while(walker) {
        walker->prepare();
        walker = walker->next_;
    }
}

