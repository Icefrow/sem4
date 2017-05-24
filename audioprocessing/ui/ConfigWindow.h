#pragma once

#include "AudioProcessor.h"

class ConfigWindow
{
public:
    ConfigWindow();

    ~ConfigWindow();

    void show();

    void hide();

private:
    void fillDataControls();

    static void showError(HWND parent, const char * text);

    void showError(const char * text);

    void onDriverSelected(int newIndex);

    void onChannelChanged();

    void onSampleRateChanged();

    void onShowControlPanel();

    void onDriverWantsReset();

    static LRESULT CALLBACK wndProc(HWND hwnd,
                                    UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam);
    HWND hwnd_;
    HWND cbxDriver_;    // select driver
    HWND cbxInput_;     // select input channel
    HWND cbxOutputL_;   // select output channel L
    HWND cbxOutputR_;   // select output channel R
    HWND cbxSRate_;     // select sample rate

    HWND btnConfig_;    // show control panel
    HWND btnSave_;      // save config
    HWND btnHide_;      // hide this window

    std::vector<DriverId> driverList_;
    int driverIndex_ = -1;
  
    //DriverCaps driverParams_;
};
