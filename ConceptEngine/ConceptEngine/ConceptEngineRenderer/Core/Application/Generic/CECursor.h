#pragma once
#include "../../../CEDefinitions.h"

#include "../../../Core/RefCountedObject.h"
#include "../../../Core/CERef.h"

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4100) // Disable unreferenced variable
#endif

class CEWindow;

class CECursor : public RefCountedObject
{
public:
    virtual ~CECursor() = default;

    virtual void* GetNativeHandle() const { return nullptr; }

public:
    static CERef<CECursor> Arrow;
    static CERef<CECursor> TextInput;
    static CERef<CECursor> ResizeAll;
    static CERef<CECursor> ResizeEW;
    static CERef<CECursor> ResizeNS;
    static CERef<CECursor> ResizeNESW;
    static CERef<CECursor> ResizeNWSE;
    static CERef<CECursor> Hand;
    static CERef<CECursor> NotAllowed;
};

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif