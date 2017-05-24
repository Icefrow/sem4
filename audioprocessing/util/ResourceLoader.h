#pragma once

struct Blob {
    void * data;
    size_t size;

    explicit Blob(void * d = nullptr, size_t s = 0)
        : data(d)
        , size(s) {}
};

Blob loadResourceData(const char * resourceName, const char * resourceType,
                      HMODULE dllOrExe = nullptr);

