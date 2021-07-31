#include "WindowsCursor.h"

GenericCursor* WindowsCursor::Create(LPCWSTR CursorName)
{
    CERef<WindowsCursor> NewCursor = DBG_NEW WindowsCursor();
    if (!NewCursor->Init(CursorName))
    {
        return nullptr;
    }
    else
    {
        return NewCursor.ReleaseOwnership();
    }
}

WindowsCursor::WindowsCursor()
    : GenericCursor()
    , Cursor(0)
    , CursorName(nullptr)
{
}

WindowsCursor::~WindowsCursor()
{
    if (!CursorName)
    {
        DestroyCursor(Cursor);
    }
}

bool WindowsCursor::Init(LPCWSTR InCursorName)
{
    CursorName = InCursorName;
    if (CursorName)
    {
        Cursor = LoadCursor(0, CursorName);
        return true;
    }
    else
    {
        return false;
    }
}