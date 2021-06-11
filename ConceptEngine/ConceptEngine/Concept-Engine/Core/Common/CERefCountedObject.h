#pragma once
#include "CETypes.h"

	class CERefCountedObject {
	public:
		CERefCountedObject();
		virtual ~CERefCountedObject() = default;

		uint32 AddRef();
		uint32 Release();

		uint32 GetRefCount() const {
			return StrongReferences;
		}

	protected:
	private:
		uint32 StrongReferences;
	};
