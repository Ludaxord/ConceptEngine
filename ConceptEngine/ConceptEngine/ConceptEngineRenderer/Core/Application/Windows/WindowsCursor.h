#pragma once
#include "../../../Core/Application/Generic/GenericCursor.h"

#include "Windows.h"

class CEWindow;

class WindowsCursor : public GenericCursor {
public:
	WindowsCursor();
	~WindowsCursor();

	bool Init(LPCWSTR InCursorName);

	virtual void* GetNativeHandle() const override final {
		return reinterpret_cast<void*>(Cursor);
	}

	HCURSOR GetHandle() const {
		return Cursor;
	}

	static GenericCursor* Create(LPCWSTR CursorName);

private:
	HCURSOR Cursor;
	LPCWSTR CursorName;
};
