#include "CEGenericPlatform.h"

PlatformCallbacks* CEGenericPlatform::Callbacks = nullptr;

void CEGenericPlatform::SetCallbacks(PlatformCallbacks* InCallbacks)
{
    Callbacks = InCallbacks;
}

PlatformCallbacks* CEGenericPlatform::GetCallbacks()
{
    return nullptr;
}
