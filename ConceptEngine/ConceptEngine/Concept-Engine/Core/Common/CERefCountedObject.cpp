#include "CERefCountedObject.h"

ConceptEngine::Core::Common::CERefCountedObject::CERefCountedObject(): StrongReferences(0) {
	AddRef();
}

uint32 ConceptEngine::Core::Common::CERefCountedObject::AddRef() {
	return ++StrongReferences;
}

uint32 ConceptEngine::Core::Common::CERefCountedObject::Release() {
	uint32 newRefCount = --StrongReferences;
	if (StrongReferences <= 0) {
		delete this;
	}

	return newRefCount;
}
