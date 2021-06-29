#include "CERefCountedObject.h"

CERefCountedObject::CERefCountedObject(): StrongReferences(0) {
	AddRef();
}

uint32 CERefCountedObject::AddRef() {
	return ++StrongReferences;
}

uint32 CERefCountedObject::Release() {
	DebugMessage();
	uint32 newRefCount = --StrongReferences;
	if (StrongReferences <= 0) {
		delete this;
	}

	return newRefCount;
}
