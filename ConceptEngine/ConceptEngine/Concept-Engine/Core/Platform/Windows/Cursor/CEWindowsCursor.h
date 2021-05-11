#pragma once

#include "../../Generic/Cursor/CECursor.h"

namespace ConceptEngine::Core::Platform::Windows::Cursor {
	class CEWindowsCursor final : public Generic::Cursor::CECursor {
	public:
		CEWindowsCursor();
		~CEWindowsCursor() override;

		bool Init(LPCWSTR cursorName);

		virtual void* GetNativeHandle() override {
			return reinterpret_cast<void*>(Cursor);
		}

		HCURSOR GetHandle() const {
			return Cursor;
		}

		static CECursor* Create(LPCWSTR cursorName);

	private:
		HCURSOR Cursor;
		LPCWSTR cursorName;
	};
}
