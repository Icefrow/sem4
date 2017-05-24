#include "pch.h"
#include "Cursor.h"

CursorHandle CursorManager::getCursorHandle(Cursor cursor)
{
    return LoadCursor(NULL, MAKEINTRESOURCE(cursor));
}
