#pragma once

class Preparable
{
public:
    static void prepareAll();

protected:
    Preparable();

    virtual ~Preparable();

    virtual void prepare() = 0;

private:
    Preparable * next_ = nullptr;
    Preparable ** prev_ = nullptr;
};

