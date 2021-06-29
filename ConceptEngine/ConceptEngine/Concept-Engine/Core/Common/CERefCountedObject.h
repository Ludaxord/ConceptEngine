#pragma once
#include "CETypes.h"

// Macro for deleting objects safley
#define SafeDelete(OutObject) \
    if ((OutObject)) \
    { \
        delete (OutObject); \
        (OutObject) = nullptr; \
    }

#define SafeRelease(OutObject) \
    if ((OutObject)) \
    { \
        (OutObject)->Release(); \
        (OutObject) = nullptr; \
    }

#define SafeAddRef(OutObject) \
    if ((OutObject)) \
    { \
        (OutObject)->AddRef(); \
    }

class CERefCountedObject {
public:
	CERefCountedObject();
	virtual ~CERefCountedObject() = default;

	uint32 AddRef();
	virtual uint32 Release();

	virtual void DebugMessage() {

	}

	uint32 GetRefCount() const {
		return StrongReferences;
	}

protected:
private:
	uint32 StrongReferences;
};
