#include "CEWindowsCursor.h"

using namespace ConceptEngine::Core::Platform::Windows::Cursor;
using namespace ConceptEngine::Core::Platform::Generic::Cursor;

CEWindowsCursor::CEWindowsCursor(): Cursor(0), CursorName(nullptr) {

}

CEWindowsCursor::~CEWindowsCursor() {
	if (!CursorName) {
		DestroyCursor(Cursor);
	}
}

bool CEWindowsCursor::Init(LPCWSTR cursorName) {
	CursorName = cursorName;
	if (CursorName) {
		Cursor = LoadCursor(0, CursorName);
		return true;
	}
	return false;
}

CECursor* CEWindowsCursor::Create(LPCWSTR cursorName) {
	CERef<CEWindowsCursor> cursor = new CEWindowsCursor();
	if (!cursor->Init(cursorName)) {
		return nullptr;
	}
	return cursor.ReleaseOwnership();
}
