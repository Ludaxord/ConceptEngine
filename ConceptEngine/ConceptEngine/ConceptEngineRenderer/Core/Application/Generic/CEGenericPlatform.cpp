#include "CEGenericPlatform.h"

CEPlatformCallbacks* CEGenericPlatform::Callbacks = nullptr;

void CEGenericPlatform::SetCallbacks(CEPlatformCallbacks* InCallbacks)
{
    Callbacks = InCallbacks;
}

CEPlatformCallbacks* CEGenericPlatform::GetCallbacks()
{
    return nullptr;
}
