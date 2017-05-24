#include "pch.h"
#include "Environment.h"

using namespace std;

namespace {
int s_argc = 0;
char ** s_argv = nullptr;
string s_programFullPath;
}

void Environment::init(int argc, char * argv[])
{
    s_argc = argc;
    s_argv = argv;
    s_programFullPath = argv[0];
}

string Environment::programFullPath()
{
    return s_programFullPath;
}

unsigned Environment::scrollBarWidth()
{
    return GetSystemMetrics(SM_CXVSCROLL);
}

unsigned Environment::dragThreshold()
{
    return GetSystemMetrics(SM_CXDRAG);
}

