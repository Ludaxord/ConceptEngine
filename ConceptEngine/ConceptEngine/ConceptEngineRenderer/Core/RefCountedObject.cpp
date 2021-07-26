#include "RefCountedObject.h"

RefCountedObject::RefCountedObject()
    : StrongReferences(0)
{
    AddRef();
}

uint32 RefCountedObject::AddRef()
{
    return ++StrongReferences;
}

uint32 RefCountedObject::Release()
{
	DebugMessage();
    uint32 NewRefCount = --StrongReferences;
    if (StrongReferences <= 0)
    {
    	if (this != nullptr) 
        {
			delete this;    		
    	}
    }

    return NewRefCount;
}
