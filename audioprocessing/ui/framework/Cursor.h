#pragma once

/* -------------------------------------------------------------- */
#define WINAPI_CURSOR(id) WORD((ULONG_PTR)id)

enum class Cursor {
    Default                 = 0,
    AppStarting             = WINAPI_CURSOR(IDC_APPSTARTING),
    Arrow                   = WINAPI_CURSOR(IDC_ARROW),
    Cross                   = WINAPI_CURSOR(IDC_CROSS),
    Hand                    = WINAPI_CURSOR(IDC_HAND),
    Help                    = WINAPI_CURSOR(IDC_HELP),
    IBeam                   = WINAPI_CURSOR(IDC_IBEAM),
    SizeAll                 = WINAPI_CURSOR(IDC_SIZEALL),
    SizeNortheastSouthwest  = WINAPI_CURSOR(IDC_SIZENESW),
    SizeNorthSouth          = WINAPI_CURSOR(IDC_SIZENS),
    SizeNorthwestSoutheast  = WINAPI_CURSOR(IDC_SIZENWSE),
    SizeWestEast            = WINAPI_CURSOR(IDC_SIZEWE),
    No             = WINAPI_CURSOR(IDC_NO),
    UpArrow                 = WINAPI_CURSOR(IDC_UPARROW),
    Wait                    = WINAPI_CURSOR(IDC_WAIT),
};

/* -------------------------------------------------------------- */
typedef HCURSOR CursorHandle;

/* -------------------------------------------------------------- */
class CursorManager final {
public:
    static CursorHandle getCursorHandle(Cursor cursor);
};