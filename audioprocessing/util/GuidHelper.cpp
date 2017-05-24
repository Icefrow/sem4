#include "pch.h"
#include "GuidHelper.h"

using namespace std;

void guidToString(const GUID & guid, GuidStr & outString)
{
    sprintf(outString, "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

void stringToGuid(const std::string & str, GUID & outGuid)
{
    if(str.size() < 38)
        throw std::invalid_argument("GUID string is short");

    sscanf(str.c_str(),
           "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
           &outGuid.Data1, &outGuid.Data2, &outGuid.Data3,
           &outGuid.Data4[0], &outGuid.Data4[1], &outGuid.Data4[2], &outGuid.Data4[3],
           &outGuid.Data4[4], &outGuid.Data4[5], &outGuid.Data4[6], &outGuid.Data4[7]);
}

ostream & operator<<(ostream & s, const GUID & guid)
{
    GuidStr str;
    guidToString(guid, str);
    s.write(str, 38);
    return s;
}