#include "CEWindowsCursor.h"

CECursor* CEWindowsCursor::Create(LPCWSTR CursorName)
{
    CERef<CEWindowsCursor> NewCursor = DBG_NEW CEWindowsCursor();
    if (!NewCursor->Init(CursorName))
    {
        return nullptr;
    }
    else
    {
        return NewCursor.ReleaseOwnership();
    }
}

CEWindowsCursor::CEWindowsCursor()
    : CECursor()
    , Cursor(0)
    , CursorName(nullptr)
{
}

CEWindowsCursor::~CEWindowsCursor()
{
    if (!CursorName)
    {
        DestroyCursor(Cursor);
    }
}

bool CEWindowsCursor::Init(LPCWSTR InCursorName)
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