#pragma once
#include "CETypes.h"

namespace ConceptEngine::Core::Common {
	class CEClassType {
	public:
		CEClassType(const char* name, const CEClassType* superClass, uint32 sizeInBytes);
		~CEClassType() = default;

		bool IsSubClassOf(const CEClassType* Class) const;

		template <typename T>
		bool IsSubClassOf() const {
			return IsSubClassOf(T::GetStaticClass());
		}

		const char* GetName() const {
			return Name;
		}

		const CEClassType* GetSuperClass() const {
			return SuperClass;
		}

		uint32 GetSizeInBytes() const {
			return SizeInBytes;
		}

	private:
		const char* Name;
		const CEClassType* SuperClass;
		const uint32 SizeInBytes;
	};
}
