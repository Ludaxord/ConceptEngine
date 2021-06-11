#include "CEClassType.h"

ConceptEngine::Core::Common::CEClassType::CEClassType(const char* name, const CEClassType* superClass,
                                                      uint32 sizeInBytes) : Name(name),
                                                                            SuperClass(superClass),
                                                                            SizeInBytes(sizeInBytes) {
}

bool ConceptEngine::Core::Common::CEClassType::IsSubClassOf(const CEClassType* Class) const {
	for (const CEClassType* current = this; current; current = current->GetSuperClass()) {
		if (current == Class) {
			return true;
		}
	}
	return false;
}
