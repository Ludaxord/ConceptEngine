#pragma once
#include "../CEDefinitions.h"

class RefCountedObject
{
public:
    RefCountedObject();
    virtual ~RefCountedObject() = default;

    uint32 AddRef();
    virtual uint32 Release();
    
	virtual void DebugMessage() {

	}
	
    uint32 GetRefCount() const { return StrongReferences; }

private:
    uint32 StrongReferences;
};