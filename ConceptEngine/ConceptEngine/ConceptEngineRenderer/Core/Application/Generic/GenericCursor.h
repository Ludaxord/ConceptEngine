#pragma once
#include "../../../CEDefinitions.h"

#include "../../../Core/RefCountedObject.h"
#include "../../../Core/CERef.h"

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(push)
    #pragma warning(disable : 4100) // Disable unreferenced variable
#endif

class CEWindow;

class GenericCursor : public RefCountedObject
{
public:
    virtual ~GenericCursor() = default;

    virtual void* GetNativeHandle() const { return nullptr; }

public:
    static CERef<GenericCursor> Arrow;
    static CERef<GenericCursor> TextInput;
    static CERef<GenericCursor> ResizeAll;
    static CERef<GenericCursor> ResizeEW;
    static CERef<GenericCursor> ResizeNS;
    static CERef<GenericCursor> ResizeNESW;
    static CERef<GenericCursor> ResizeNWSE;
    static CERef<GenericCursor> Hand;
    static CERef<GenericCursor> NotAllowed;
};

#ifdef COMPILER_VISUAL_STUDIO
    #pragma warning(pop)
#endif