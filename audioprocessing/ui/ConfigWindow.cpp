#include "pch.h"

#include <Windowsx.h>
#include <winuser.h>

#include <vector>
#include <sstream>
#include <strstream>

#include "res/resource.h"
#include "util/GuidHelper.h"

#include "AudioProcessor.h"
#include "ConfigWindow.h"

using namespace std;

//extern Oscilloscop * s_inputOsc;
//extern Oscilloscop * s_outputOsc;

// IDs of controls
enum {
    COMBO_DRIVERS = 1,
    COMBO_INPUTS,
    COMBO_OUTPUTS_L,
    COMBO_OUTPUTS_R,
    COMBO_SAMPLE_RATE,
    BUTTON_SHOW_CONTROL_PANEL,
    BUTTON_SAVE,
    BUTTON_HIDE,
    OSC_INPUT,
    OSC_OUTPUT,
};

void ConfigWindow::onDriverWantsReset()
{
    cout << "Driver wants to be reset..." << endl;

    try {
        AudioParams copyOfParams = AudioProcessor::audioParams();
        AudioProcessor::openDriver(driverList_[driverIndex_].clsid, false);
        AudioProcessor::changeParams(copyOfParams);
        fillDataControls();
    }
    catch(exception & ex) {
        fillDataControls();
        showError(ex.what());
    }
}

void ConfigWindow::fillDataControls()
{
    ComboBox_ResetContent(cbxInput_);
    ComboBox_ResetContent(cbxOutputL_);
    ComboBox_ResetContent(cbxOutputR_);
    ComboBox_ResetContent(cbxSRate_);

    const DriverInfo & driverInfo = AudioProcessor::driverInfo();
    const AudioParams & params = AudioProcessor::audioParams();

    //driver_.reset();    // test when driver failed or no channels
    if(driverInfo.inputChannels.size() && driverInfo.outputChannels.size()) {
        for(const ChannelInfo & chanInfo : driverInfo.inputChannels)
            ComboBox_AddString(cbxInput_, chanInfo.name.c_str());

        ComboBox_SetCurSel(cbxInput_, params.inputChannel);
        EnableWindow(cbxInput_, TRUE);

        for(const ChannelInfo & chanInfo : driverInfo.outputChannels) {
            const char * channelName = chanInfo.name.c_str();
            ComboBox_AddString(cbxOutputL_, channelName);
            ComboBox_AddString(cbxOutputR_, channelName);
        }

        ComboBox_SetCurSel(cbxOutputL_, params.outputChannelL);
        EnableWindow(cbxOutputL_, TRUE);

        ComboBox_SetCurSel(cbxOutputR_, params.outputChannelR);
        EnableWindow(cbxOutputR_, TRUE);

        int currentSampleRateIndex = -1;
        int i = 0;

        for(const double sampleRate : driverInfo.supportedSampleRates) {
            strstream buffer;
            buffer << sampleRate;
            buffer.write("", 1);
            ComboBox_AddString(cbxSRate_, buffer.str());

            if(sampleRate == AudioProcessor::sampleRate())
                currentSampleRateIndex = i;
            else
                i++;
        }

        ComboBox_SetCurSel(cbxSRate_, currentSampleRateIndex);
        EnableWindow(cbxSRate_, TRUE);
        EnableWindow(btnSave_, TRUE);
    }
    else {
        // driver failed or no input/output channels
        ComboBox_AddString(cbxInput_, "No input or output channels!");

        ComboBox_SetCurSel(cbxInput_, 0);
        EnableWindow(cbxInput_, FALSE);

        ComboBox_SetCurSel(cbxOutputL_, -1);
        EnableWindow(cbxOutputL_, FALSE);

        ComboBox_SetCurSel(cbxOutputR_, -1);
        EnableWindow(cbxOutputR_, FALSE);

        ComboBox_SetCurSel(cbxSRate_, -1);
        EnableWindow(cbxSRate_, FALSE);

        EnableWindow(btnSave_, FALSE);
    }
}

void ConfigWindow::onDriverSelected(int newIndex)
{
    assert(newIndex >= 0 && newIndex < (int)driverList_.size());

    if(newIndex == driverIndex_)
        return;

    driverIndex_ = newIndex;
    const CLSID & clsid = driverList_[newIndex].clsid;

    if(AudioProcessor::driverInfo().clsid == clsid)
        return;

    try {
        AudioProcessor::openDriver(clsid);
        fillDataControls();
//        onChannelChanged();
    }
    catch(exception & ex) {
        fillDataControls();
        showError(ex.what());
    }
}

void ConfigWindow::onChannelChanged()
{
    AudioParams params = AudioProcessor::audioParams();

    params.inputChannel = ComboBox_GetCurSel(cbxInput_);

    params.outputChannelL = ComboBox_GetCurSel(cbxOutputL_);
    params.outputChannelR = ComboBox_GetCurSel(cbxOutputR_);

    try {
        AudioProcessor::changeParams(params);
    }
    catch(const exception & ex) {
        showError(ex.what());
    }
}

void ConfigWindow::onSampleRateChanged()
{
    int rateIndex = ComboBox_GetCurSel(cbxSRate_);

    const DriverInfo & driverInfo = AudioProcessor::driverInfo();
    double sampleRate = driverInfo.supportedSampleRates[rateIndex];

    try {
        AudioProcessor::setSampleRate(sampleRate);
    }
    catch(const exception & ex) {
        showError(ex.what());
    }
}

void ConfigWindow::onShowControlPanel()
{
    try {
        AudioProcessor::showControlPanel();
    }
    catch(exception & ex) {
        showError(ex.what());
    }
}

//void ConfigWindow::save()
//{
//    //GuidStr driverId;
//    //guidToString(driverList_[driverIndex_].clsid, driverId);
//
//    //int rateIndex = ComboBox_GetCurSel(cbxSRate_);
//    //double rate = driver_->supportedSampleRates()[rateIndex];
//
//    //int inputChanIndex = ComboBox_GetCurSel(cbxInput_);
//    //string inputChan = driver_->inputChannels()[inputChanIndex].name;
//
//    //int outChanLIndex = ComboBox_GetCurSel(cbxOutputL_);
//    //string outChanL = driver_->outputChannels()[outChanLIndex].name;
//
//    //int outChanRIndex = ComboBox_GetCurSel(cbxOutputR_);
//    //string outChanR = driver_->outputChannels()[outChanRIndex].name;
//    return;
//
//
//    auto enableControls = [this](BOOL enable) {
//        for(HWND w : {
//                cbxDriver_,
//                cbxInput_,
//                cbxOutputL_,
//                cbxOutputR_,
//                cbxSRate_,
//                btnConfig_
//            }) EnableWindow(w, enable);
//    };
//
//    if(started_) {
//        driver_->stop();
//        // do stop
//        started_ = false;
//        Button_SetText(btnSave_, "START");
//        enableControls(TRUE);
//    }
//    else {
//        // do start
//        int inputChan = ComboBox_GetCurSel(cbxInput_);
//        int outChanL = ComboBox_GetCurSel(cbxOutputL_);
//        int outChanR = ComboBox_GetCurSel(cbxOutputR_);
//
//        try {
//            driver_->start(inputChan, outChanL, outChanR);
//        }
//        catch(const exception & ex) {
//            showError(ex.what());
//            return;
//        }
//
//        started_ = true;
//        Button_SetText(btnSave_, "STOP");
//        enableControls(FALSE);
//    }
//
//    //RedrawWindow(ledStartStop_, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
//}

///////////////////////////////////////////////////////////////////////////////////////
// Windows API specific code

namespace {

char MainWindowsClassName[] = "MainWindows";
char ApplicationName[] = "Music FX";
const HINSTANCE hInstance = GetModuleHandle(NULL);
HBITMAP imgBackground = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP_BK));
HBRUSH imgStarted = CreatePatternBrush(LoadBitmap(hInstance,
                                                  MAKEINTRESOURCE(IDB_BITMAP_STARTED)));
HBRUSH imgStopped = CreatePatternBrush(LoadBitmap(hInstance,
                                                  MAKEINTRESOURCE(IDB_BITMAP_STOPPED)));

bool registerMainWindowClass(WNDPROC wndProc)
{
    static bool registered = false;

    if(registered)
        return true;

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreatePatternBrush(imgBackground);
    //wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = MainWindowsClassName;
    wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

    if(!RegisterClassEx(&wcex)) {
        MessageBox(NULL, "Call to RegisterClassEx failed!",
                   ApplicationName, NULL);

        return false;
    }

    registered = true;
    return true;
}

#ifndef GWLP_USERDATA
#define GWLP_USERDATA       (-21)
#endif

void setWindowRef(HWND hwnd, void * windowObj)
{
    SetWindowLongPtr(hwnd, GWLP_USERDATA, LONG_PTR(windowObj));
}

template<class T>
T * getWindowRef(HWND hwnd)
{
    return reinterpret_cast<T *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
}

HWND addChild(HWND parent, char * className, char * text, DWORD style, POINT point,
              SIZE size, int id)
{
    return ::CreateWindow(className, text, style | WS_CHILD | WS_VISIBLE, point.x, point.y,
                          size.cx, size.cy, parent, HMENU(UINT_PTR(id)), hInstance, NULL);
}

WNDPROC defStaticTextWndProc = NULL;

HWND addStatic(HWND parent, char * text, POINT point, SIZE size, int id = 0)
{
    return addChild(parent, "STATIC", text, 0, point, size, id);
}

HWND addStatic(HWND parent, char * text, DWORD style, POINT point, SIZE size, int id = 0)
{
    return addChild(parent, "STATIC", text, style, point, size, id);
}

HWND addComboBox(HWND parent, POINT point, SIZE size, int id)
{
    return addChild(parent, "COMBOBOX", "", CBS_DROPDOWNLIST, point, size, id);
}

HWND addButton(HWND parent, char * text, POINT point, SIZE size, int id)
{
    return addChild(parent, "BUTTON", text, 0, point, size, id);
}

int getDriverIndex(const vector<DriverId> & drivers, const GUID & driverId)
{
    for(int i = 0, size = drivers.size(); i < size; i++) {
        if(drivers[i].clsid == driverId)
            return i;
    }

    throw runtime_error("Driver " + guidToString(driverId) + " not found");
}

int getChannelIndex(const vector<ChannelInfo> & channels, const string & channelName)
{
    for(int i = 0, size = channels.size(); i < size; i++) {
        if(channels[i].name == channelName)
            return i;
    }

    throw runtime_error("Channel \"" + channelName + "\" not found");
}

template<typename T>
int getElementIndex(const vector<T> & list, const T & element)
{
    for(int i = 0, size = list.size(); i < size; i++) {
        if(list[i] == element)
            return i;
    }

    stringstream elementAsText;
    elementAsText << element;

    throw runtime_error("Element \"" + elementAsText.str() + "\" not found");
}

} // namespace


ConfigWindow::ConfigWindow()
    : hwnd_(NULL)
    , driverIndex_(-1)
{
    if(!registerMainWindowClass(wndProc)) {
        PostQuitMessage(-1);
        return;
    }

    BITMAP bm;
    GetObject(imgBackground, sizeof(bm), &bm);
    RECT wndSize = {0, 0, bm.bmWidth, bm.bmHeight};

    DWORD wndStyle = WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION |
                     WS_SYSMENU | WS_MINIMIZEBOX;

    AdjustWindowRect(&wndSize, wndStyle, FALSE);

    hwnd_ = CreateWindow(MainWindowsClassName, ApplicationName, wndStyle,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         wndSize.right - wndSize.left, wndSize.bottom - wndSize.top,
                         NULL, NULL, hInstance, NULL);

    if(!hwnd_) {
        showError(NULL, "Call to CreateWindow failed!");
        PostQuitMessage(-1);
        return;
    }

    SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    cbxDriver_ = addComboBox(hwnd_, {108, 43}, {250, 500}, COMBO_DRIVERS);

    bool previousConfigIsOK = AudioProcessor::init(hwnd_);
    AudioProcessor::getDriverList(driverList_);

    // drivers_.clear();     // test when no drivers found
    if(driverList_.size()) {
        for(const DriverId & driverInfo : driverList_)
            ComboBox_AddString(cbxDriver_, driverInfo.name.c_str());

        btnConfig_ = addButton(hwnd_, "Configure...", {371, 42}, {90, 26},
                               BUTTON_SHOW_CONTROL_PANEL);

        cbxInput_ = addComboBox(hwnd_, {108, 81}, {250, 500}, COMBO_INPUTS);
        cbxOutputL_ = addComboBox(hwnd_, {108, 119}, {250, 500}, COMBO_OUTPUTS_L);
        cbxOutputR_ = addComboBox(hwnd_, {108, 157}, {250, 500}, COMBO_OUTPUTS_R);
        cbxSRate_ = addComboBox(hwnd_, {372, 81}, {88, 500}, COMBO_SAMPLE_RATE);

        btnSave_ = addButton(hwnd_, "&Save", {268, 223}, {90, 26}, BUTTON_SAVE);

        driverIndex_ = getDriverIndex(driverList_, AudioProcessor::driverInfo().clsid);
        ComboBox_SetCurSel(cbxDriver_, driverIndex_);
        fillDataControls();
        //onDriverSelected(index);
    }
    else {
        // drivers_.size() == 0
        ComboBox_AddString(cbxDriver_, "No ASIO drivers found!");
        ComboBox_SetCurSel(cbxDriver_, 0);
        EnableWindow(cbxDriver_, FALSE);
    }

    btnHide_ = addButton(hwnd_, "&Hide", { 371, 223 }, { 90, 26 }, BUTTON_HIDE);
    setWindowRef(hwnd_, this);

    if(!previousConfigIsOK)
        show();
}

ConfigWindow::~ConfigWindow()
{
    AudioProcessor::closeDriver();

    if(hwnd_)
        DestroyWindow(hwnd_);
}

void ConfigWindow::show()
{
    ShowWindow(hwnd_, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_);
}

void ConfigWindow::hide()
{
    ShowWindow(hwnd_, SW_HIDE);
}

void ConfigWindow::showError(HWND parent, const char * text)
{
    MessageBox(parent, text, ApplicationName, MB_OK | MB_ICONWARNING);
}

void ConfigWindow::showError(const char * text)
{
    showError(hwnd_, text);
}

//void ConfigWindow::run()
//{
//    MSG msg;
//
//    while(GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//}

LRESULT CALLBACK ConfigWindow::wndProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam)
{
    ConfigWindow * wnd = getWindowRef<ConfigWindow>(hwnd);

    switch(message) {
    case WM_CLOSE:
        wnd->hide();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 1;

    case WM_COMMAND: {
        unsigned cmd = LOWORD(wParam);

        switch(cmd) {
        case BUTTON_SHOW_CONTROL_PANEL:
            wnd->onShowControlPanel();
            return TRUE;

        case BUTTON_SAVE:
            AudioProcessor::saveConfig();
            wnd->hide();
            return TRUE;

        case BUTTON_HIDE:
            wnd->hide();
            return TRUE;

        case CMD_RESET_DEVICE:
            wnd->onDriverWantsReset();
            return TRUE;

            //case Oscilloscop::CMD_INVALIDATE:
            //    InvalidateRect((HWND)lParam, NULL, TRUE);
            //    return TRUE;
        }

        if(HIWORD(wParam) == CBN_SELCHANGE) {
            switch(cmd) {
            case COMBO_DRIVERS:
                wnd->onDriverSelected(ComboBox_GetCurSel((HWND)lParam));
                return TRUE;

            case COMBO_INPUTS:
            case COMBO_OUTPUTS_L:
            case COMBO_OUTPUTS_R:
                wnd->onChannelChanged();
                return TRUE;

            case COMBO_SAMPLE_RATE:
                wnd->onSampleRateChanged();
                return TRUE;
            }
        }
    }

        //case WM_CTLCOLORSTATIC:
        //    if((HWND)lParam == mainWindow->ledStartStop_)
        //        return (INT_PTR)(mainWindow->started_ ? imgStarted : imgStopped);

        //    break;

        //case WM_DRAWITEM: {
        //    DRAWITEMSTRUCT * ds = (DRAWITEMSTRUCT *)lParam;
        //    Oscilloscop * osc = getWindowRef<Oscilloscop>(ds->hwndItem);
        //    osc->paint(ds->hDC, ds->rcItem);
        //}
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}