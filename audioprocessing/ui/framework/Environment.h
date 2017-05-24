#pragma once

#include <string>

// Helper class
class Environment
{
public:
    static void init(int argc, char *argv[]);

    static std::string programFullPath();

    static std::string programDir();

    static unsigned int scrollBarWidth();

    // The number of pixels on either side of a mouse-down point that the mouse pointer can move before a drag operation begins. 
    // This allows the user to click and release the mouse button easily without unintentionally starting a drag operation. 
    static unsigned int dragThreshold();

private:
    Environment() = delete;
    ~Environment() = delete;
};

