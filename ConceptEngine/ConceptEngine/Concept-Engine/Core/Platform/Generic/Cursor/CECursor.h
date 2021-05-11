#pragma once

#include "../../../Common/CERefCountedObject.h"
#include "../../../Common/CERef.h"

namespace ConceptEngine::Core::Platform::Generic::Cursor {
	class CECursor : public Common::CERefCountedObject {
	public:
		virtual ~CECursor() = default;

		virtual void* GetNativeHandle() {
			return nullptr;
		}

		static Common::CERef<CECursor> Arrow;
		static Common::CERef<CECursor> TextInput;
		static Common::CERef<CECursor> ResizeAll;
		static Common::CERef<CECursor> ResizeEW;
		static Common::CERef<CECursor> ResizeNS;
		static Common::CERef<CECursor> ResizeNESW;
		static Common::CERef<CECursor> ResizeNWSE;
		static Common::CERef<CECursor> Hand;
		static Common::CERef<CECursor> NotAllowed;
	};
}
