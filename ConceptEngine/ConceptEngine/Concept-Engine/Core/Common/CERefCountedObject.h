#pragma once
#include "CETypes.h"

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
