// Some GUID helpers

#pragma once

#include <GuidDef.h>
#include <ostream>

// char buffer for guid
typedef char GuidStr[39];

void guidToString(const GUID & guid, GuidStr & outString);

inline
std::string guidToString(const GUID & guid)
{
    GuidStr result;
    guidToString(guid, result);
    return result;
}

void stringToGuid(const std::string & str, GUID & outGuid);

std::ostream & operator<<(std::ostream & s, const GUID & guid);
