// MusicFX.cpp : Defines the entry point for the console application.
//

#include "pch.h"

#include <ui/framework/HostWindow.h>
#include <ui/MainForm.h>

#include "ui/framework/Environment.h"

using namespace std;

int main(int argc, char *argv[])
{
    Environment::init(argc, argv);

    setlocale(LC_ALL, "Rus");
    CoInitialize(0);    // init COM

    // test line for commit

    // do not go sleep in multimedia application
    SetThreadExecutionState(ES_AWAYMODE_REQUIRED | ES_DISPLAY_REQUIRED | ES_CONTINUOUS);

    try {
        cout << "-= MusicFX sample application =-" << endl;

        sf::ContextSettings settings;
        settings.antialiasingLevel = 2;

        MainForm mainForm;
        HostWindow mainWindow("MusicFX", &mainForm, settings);

        mainWindow.run();
    }
    catch(const exception & ex) {
        cout << "Error: " << ex.what() << endl;
    }

    ResourceManager::clear();

    CoUninitialize();
    return 0;
}

