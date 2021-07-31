#pragma once
#include "../../../Core/Application/Generic/CECursor.h"

#include "Windows.h"

class CEWindow;

class CEWindowsCursor : public CECursor {
public:
	CEWindowsCursor();
	~CEWindowsCursor();

	bool Init(LPCWSTR InCursorName);

	virtual void* GetNativeHandle() const override final {
		return reinterpret_cast<void*>(Cursor);
	}

	HCURSOR GetHandle() const {
		return Cursor;
	}

	static CECursor* Create(LPCWSTR CursorName);

private:
	HCURSOR Cursor;
	LPCWSTR CursorName;
};
