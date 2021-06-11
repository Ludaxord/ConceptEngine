#pragma once

#include "../../../Common/CERefCountedObject.h"
#include "../../../Common/CERef.h"

namespace ConceptEngine::Core::Platform::Generic::Cursor {
	class CECursor : public CERefCountedObject {
	public:
		virtual ~CECursor() = default;

		virtual void* GetNativeHandle() {
			return nullptr;
		}

		static CERef<CECursor> Arrow;
		static CERef<CECursor> TextInput;
		static CERef<CECursor> ResizeAll;
		static CERef<CECursor> ResizeEW;
		static CERef<CECursor> ResizeNS;
		static CERef<CECursor> ResizeNESW;
		static CERef<CECursor> ResizeNWSE;
		static CERef<CECursor> Hand;
		static CERef<CECursor> NotAllowed;
	};
}
