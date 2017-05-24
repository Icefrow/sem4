#include "pch.h"

#include "ResourceLoader.h"

Blob loadResourceData(const char * resourceName,
                              const char * resourceType,
                              HMODULE dllOrExe)
{
    if (!dllOrExe)
        dllOrExe = GetModuleHandle(nullptr);

    HRSRC resHandle = FindResource(dllOrExe, resourceName, resourceType);

    if(!resHandle)
        return Blob();

    DWORD size = SizeofResource(dllOrExe, resHandle);

    if(!size)
        return Blob();

    HGLOBAL resData = LoadResource(dllOrExe, resHandle);

    if(!resData)
        return Blob();

    return Blob(LockResource(resData), size);
}
