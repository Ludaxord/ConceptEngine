#include "CEWindowsInputManager.h"

#include "../../../../Memory/CEMemory.h"

#include "../../../Debug/CEDebug.h"

using namespace ConceptEngine::Core::Platform::Windows::Input;

CEWindowsInputManager::CEWindowsInputManager() {
}

CEWindowsInputManager::~CEWindowsInputManager() {
}

bool CEWindowsInputManager::Create() {
	if (!CEInputManager::Create()) {
		CEDebug::DebugBreak();
		return false;
	}

	return true;
}

void CEWindowsInputManager::InitKeyTable() {
	Memory::CEMemory::Memzero(KeyStates.Data(), KeyStates.SizeInBytes());
	Memory::CEMemory::Memzero(ScanCodeTable.Data(), ScanCodeTable.SizeInBytes());
	Memory::CEMemory::Memzero(KeyTable.Data(), KeyTable.SizeInBytes());

	CreateNumScanCodeTableKeys();
	CreateCharScanCodeTableKeys();
	CreateFunctionScanCodeTableKeys();
	CreateNumPadScanCodeTableKeys();
	CreateActionScanCodeTableKeys();

	for (uint16 index = 0; index < 512; index++) {
		if (ScanCodeTable[index] != Generic::Input::CEKey::Key_Unknown) {
			KeyTable[ScanCodeTable[index]] = index;
		}
	}
}

void CEWindowsInputManager::CreateNumScanCodeTableKeys() {
	ScanCodeTable[0x00B] = Generic::Input::CEKey::Key_0;
	ScanCodeTable[0x002] = Generic::Input::CEKey::Key_1;
	ScanCodeTable[0x003] = Generic::Input::CEKey::Key_2;
	ScanCodeTable[0x004] = Generic::Input::CEKey::Key_3;
	ScanCodeTable[0x005] = Generic::Input::CEKey::Key_4;
	ScanCodeTable[0x006] = Generic::Input::CEKey::Key_5;
	ScanCodeTable[0x007] = Generic::Input::CEKey::Key_6;
	ScanCodeTable[0x008] = Generic::Input::CEKey::Key_7;
	ScanCodeTable[0x009] = Generic::Input::CEKey::Key_8;
	ScanCodeTable[0x00A] = Generic::Input::CEKey::Key_9;
}

void CEWindowsInputManager::CreateCharScanCodeTableKeys() {
	ScanCodeTable[0x01E] = Generic::Input::CEKey::Key_A;
	ScanCodeTable[0x030] = Generic::Input::CEKey::Key_B;
	ScanCodeTable[0x02E] = Generic::Input::CEKey::Key_C;
	ScanCodeTable[0x020] = Generic::Input::CEKey::Key_D;
	ScanCodeTable[0x012] = Generic::Input::CEKey::Key_E;
	ScanCodeTable[0x021] = Generic::Input::CEKey::Key_F;
	ScanCodeTable[0x022] = Generic::Input::CEKey::Key_G;
	ScanCodeTable[0x023] = Generic::Input::CEKey::Key_H;
	ScanCodeTable[0x017] = Generic::Input::CEKey::Key_I;
	ScanCodeTable[0x024] = Generic::Input::CEKey::Key_J;
	ScanCodeTable[0x025] = Generic::Input::CEKey::Key_K;
	ScanCodeTable[0x026] = Generic::Input::CEKey::Key_L;
	ScanCodeTable[0x032] = Generic::Input::CEKey::Key_M;
	ScanCodeTable[0x031] = Generic::Input::CEKey::Key_N;
	ScanCodeTable[0x018] = Generic::Input::CEKey::Key_O;
	ScanCodeTable[0x019] = Generic::Input::CEKey::Key_P;
	ScanCodeTable[0x010] = Generic::Input::CEKey::Key_Q;
	ScanCodeTable[0x013] = Generic::Input::CEKey::Key_R;
	ScanCodeTable[0x01F] = Generic::Input::CEKey::Key_S;
	ScanCodeTable[0x014] = Generic::Input::CEKey::Key_T;
	ScanCodeTable[0x016] = Generic::Input::CEKey::Key_U;
	ScanCodeTable[0x02F] = Generic::Input::CEKey::Key_V;
	ScanCodeTable[0x011] = Generic::Input::CEKey::Key_W;
	ScanCodeTable[0x02D] = Generic::Input::CEKey::Key_X;
	ScanCodeTable[0x015] = Generic::Input::CEKey::Key_Y;
	ScanCodeTable[0x02C] = Generic::Input::CEKey::Key_Z;
}

void CEWindowsInputManager::CreateFunctionScanCodeTableKeys() {
	ScanCodeTable[0x03B] = Generic::Input::CEKey::Key_F1;
	ScanCodeTable[0x03C] = Generic::Input::CEKey::Key_F2;
	ScanCodeTable[0x03D] = Generic::Input::CEKey::Key_F3;
	ScanCodeTable[0x03E] = Generic::Input::CEKey::Key_F4;
	ScanCodeTable[0x03F] = Generic::Input::CEKey::Key_F5;
	ScanCodeTable[0x040] = Generic::Input::CEKey::Key_F6;
	ScanCodeTable[0x041] = Generic::Input::CEKey::Key_F7;
	ScanCodeTable[0x042] = Generic::Input::CEKey::Key_F8;
	ScanCodeTable[0x043] = Generic::Input::CEKey::Key_F9;
	ScanCodeTable[0x044] = Generic::Input::CEKey::Key_F10;
	ScanCodeTable[0x057] = Generic::Input::CEKey::Key_F11;
	ScanCodeTable[0x058] = Generic::Input::CEKey::Key_F12;
	ScanCodeTable[0x064] = Generic::Input::CEKey::Key_F13;
	ScanCodeTable[0x065] = Generic::Input::CEKey::Key_F14;
	ScanCodeTable[0x066] = Generic::Input::CEKey::Key_F15;
	ScanCodeTable[0x067] = Generic::Input::CEKey::Key_F16;
	ScanCodeTable[0x068] = Generic::Input::CEKey::Key_F17;
	ScanCodeTable[0x069] = Generic::Input::CEKey::Key_F18;
	ScanCodeTable[0x06A] = Generic::Input::CEKey::Key_F19;
	ScanCodeTable[0x06B] = Generic::Input::CEKey::Key_F20;
	ScanCodeTable[0x06C] = Generic::Input::CEKey::Key_F21;
	ScanCodeTable[0x06D] = Generic::Input::CEKey::Key_F22;
	ScanCodeTable[0x06E] = Generic::Input::CEKey::Key_F23;
	ScanCodeTable[0x076] = Generic::Input::CEKey::Key_F24;
}

void CEWindowsInputManager::CreateNumPadScanCodeTableKeys() {
	ScanCodeTable[0x052] = Generic::Input::CEKey::Key_Keypad0;
	ScanCodeTable[0x04F] = Generic::Input::CEKey::Key_Keypad1;
	ScanCodeTable[0x050] = Generic::Input::CEKey::Key_Keypad2;
	ScanCodeTable[0x051] = Generic::Input::CEKey::Key_Keypad3;
	ScanCodeTable[0x04B] = Generic::Input::CEKey::Key_Keypad4;
	ScanCodeTable[0x04C] = Generic::Input::CEKey::Key_Keypad5;
	ScanCodeTable[0x04D] = Generic::Input::CEKey::Key_Keypad6;
	ScanCodeTable[0x047] = Generic::Input::CEKey::Key_Keypad7;
	ScanCodeTable[0x048] = Generic::Input::CEKey::Key_Keypad8;
	ScanCodeTable[0x049] = Generic::Input::CEKey::Key_Keypad9;
	ScanCodeTable[0x04E] = Generic::Input::CEKey::Key_KeypadAdd;
	ScanCodeTable[0x053] = Generic::Input::CEKey::Key_KeypadDecimal;
	ScanCodeTable[0x135] = Generic::Input::CEKey::Key_KeypadDivide;
	ScanCodeTable[0x11C] = Generic::Input::CEKey::Key_KeypadEnter;
	ScanCodeTable[0x059] = Generic::Input::CEKey::Key_KeypadEqual;
	ScanCodeTable[0x037] = Generic::Input::CEKey::Key_KeypadMultiply;
	ScanCodeTable[0x04A] = Generic::Input::CEKey::Key_KeypadSubtract;
}

void CEWindowsInputManager::CreateActionScanCodeTableKeys() {
	ScanCodeTable[0x02A] = Generic::Input::CEKey::Key_LeftShift;
	ScanCodeTable[0x036] = Generic::Input::CEKey::Key_RightShift;
	ScanCodeTable[0x01D] = Generic::Input::CEKey::Key_LeftControl;
	ScanCodeTable[0x11D] = Generic::Input::CEKey::Key_RightControl;
	ScanCodeTable[0x038] = Generic::Input::CEKey::Key_LeftAlt;
	ScanCodeTable[0x138] = Generic::Input::CEKey::Key_RightAlt;
	ScanCodeTable[0x15B] = Generic::Input::CEKey::Key_LeftSuper;
	ScanCodeTable[0x15C] = Generic::Input::CEKey::Key_RightSuper;
	ScanCodeTable[0x15D] = Generic::Input::CEKey::Key_Menu;
	ScanCodeTable[0x039] = Generic::Input::CEKey::Key_Space;
	ScanCodeTable[0x028] = Generic::Input::CEKey::Key_Apostrophe;
	ScanCodeTable[0x033] = Generic::Input::CEKey::Key_Comma;
	ScanCodeTable[0x00C] = Generic::Input::CEKey::Key_Minus;
	ScanCodeTable[0x034] = Generic::Input::CEKey::Key_Period;
	ScanCodeTable[0x035] = Generic::Input::CEKey::Key_Slash;
	ScanCodeTable[0x027] = Generic::Input::CEKey::Key_Semicolon;
	ScanCodeTable[0x00D] = Generic::Input::CEKey::Key_Equal;
	ScanCodeTable[0x01A] = Generic::Input::CEKey::Key_LeftBracket;
	ScanCodeTable[0x02B] = Generic::Input::CEKey::Key_Backslash;
	ScanCodeTable[0x01B] = Generic::Input::CEKey::Key_RightBracket;
	ScanCodeTable[0x029] = Generic::Input::CEKey::Key_GraveAccent;
	ScanCodeTable[0x056] = Generic::Input::CEKey::Key_World2;
	ScanCodeTable[0x001] = Generic::Input::CEKey::Key_Escape;
	ScanCodeTable[0x01C] = Generic::Input::CEKey::Key_Enter;
	ScanCodeTable[0x00F] = Generic::Input::CEKey::Key_Tab;
	ScanCodeTable[0x00E] = Generic::Input::CEKey::Key_Backspace;
	ScanCodeTable[0x152] = Generic::Input::CEKey::Key_Insert;
	ScanCodeTable[0x153] = Generic::Input::CEKey::Key_Delete;
	ScanCodeTable[0x14D] = Generic::Input::CEKey::Key_Right;
	ScanCodeTable[0x14B] = Generic::Input::CEKey::Key_Left;
	ScanCodeTable[0x150] = Generic::Input::CEKey::Key_Down;
	ScanCodeTable[0x148] = Generic::Input::CEKey::Key_Up;
	ScanCodeTable[0x149] = Generic::Input::CEKey::Key_PageUp;
	ScanCodeTable[0x151] = Generic::Input::CEKey::Key_PageDown;
	ScanCodeTable[0x147] = Generic::Input::CEKey::Key_Home;
	ScanCodeTable[0x14F] = Generic::Input::CEKey::Key_End;
	ScanCodeTable[0x03A] = Generic::Input::CEKey::Key_CapsLock;
	ScanCodeTable[0x046] = Generic::Input::CEKey::Key_ScrollLock;
	ScanCodeTable[0x145] = Generic::Input::CEKey::Key_NumLock;
	ScanCodeTable[0x137] = Generic::Input::CEKey::Key_PrintScreen;
	ScanCodeTable[0x146] = Generic::Input::CEKey::Key_Pause;
}
