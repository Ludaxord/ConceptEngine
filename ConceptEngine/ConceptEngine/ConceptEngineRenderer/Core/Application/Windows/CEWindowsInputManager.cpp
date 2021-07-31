#include "CEWindowsInputManager.h"

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
	CEMemory::Memzero(KeyStates.Data(), KeyStates.SizeInBytes());
	CEMemory::Memzero(ScanCodeTable.Data(), ScanCodeTable.SizeInBytes());
	CEMemory::Memzero(KeyTable.Data(), KeyTable.SizeInBytes());

	CreateNumScanCodeTableKeys();
	CreateCharScanCodeTableKeys();
	CreateFunctionScanCodeTableKeys();
	CreateNumPadScanCodeTableKeys();
	CreateActionScanCodeTableKeys();

	for (uint16 index = 0; index < 512; index++) {
		if (ScanCodeTable[index] != CEKey::Key_Unknown) {
			KeyTable[ScanCodeTable[index]] = index;
		}
	}
}

void CEWindowsInputManager::CreateNumScanCodeTableKeys() {
	ScanCodeTable[0x00B] = CEKey::Key_0;
	ScanCodeTable[0x002] = CEKey::Key_1;
	ScanCodeTable[0x003] = CEKey::Key_2;
	ScanCodeTable[0x004] = CEKey::Key_3;
	ScanCodeTable[0x005] = CEKey::Key_4;
	ScanCodeTable[0x006] = CEKey::Key_5;
	ScanCodeTable[0x007] = CEKey::Key_6;
	ScanCodeTable[0x008] = CEKey::Key_7;
	ScanCodeTable[0x009] = CEKey::Key_8;
	ScanCodeTable[0x00A] = CEKey::Key_9;
}

void CEWindowsInputManager::CreateCharScanCodeTableKeys() {
	ScanCodeTable[0x01E] = CEKey::Key_A;
	ScanCodeTable[0x030] = CEKey::Key_B;
	ScanCodeTable[0x02E] = CEKey::Key_C;
	ScanCodeTable[0x020] = CEKey::Key_D;
	ScanCodeTable[0x012] = CEKey::Key_E;
	ScanCodeTable[0x021] = CEKey::Key_F;
	ScanCodeTable[0x022] = CEKey::Key_G;
	ScanCodeTable[0x023] = CEKey::Key_H;
	ScanCodeTable[0x017] = CEKey::Key_I;
	ScanCodeTable[0x024] = CEKey::Key_J;
	ScanCodeTable[0x025] = CEKey::Key_K;
	ScanCodeTable[0x026] = CEKey::Key_L;
	ScanCodeTable[0x032] = CEKey::Key_M;
	ScanCodeTable[0x031] = CEKey::Key_N;
	ScanCodeTable[0x018] = CEKey::Key_O;
	ScanCodeTable[0x019] = CEKey::Key_P;
	ScanCodeTable[0x010] = CEKey::Key_Q;
	ScanCodeTable[0x013] = CEKey::Key_R;
	ScanCodeTable[0x01F] = CEKey::Key_S;
	ScanCodeTable[0x014] = CEKey::Key_T;
	ScanCodeTable[0x016] = CEKey::Key_U;
	ScanCodeTable[0x02F] = CEKey::Key_V;
	ScanCodeTable[0x011] = CEKey::Key_W;
	ScanCodeTable[0x02D] = CEKey::Key_X;
	ScanCodeTable[0x015] = CEKey::Key_Y;
	ScanCodeTable[0x02C] = CEKey::Key_Z;
}

void CEWindowsInputManager::CreateFunctionScanCodeTableKeys() {
	ScanCodeTable[0x03B] = CEKey::Key_F1;
	ScanCodeTable[0x03C] = CEKey::Key_F2;
	ScanCodeTable[0x03D] = CEKey::Key_F3;
	ScanCodeTable[0x03E] = CEKey::Key_F4;
	ScanCodeTable[0x03F] = CEKey::Key_F5;
	ScanCodeTable[0x040] = CEKey::Key_F6;
	ScanCodeTable[0x041] = CEKey::Key_F7;
	ScanCodeTable[0x042] = CEKey::Key_F8;
	ScanCodeTable[0x043] = CEKey::Key_F9;
	ScanCodeTable[0x044] = CEKey::Key_F10;
	ScanCodeTable[0x057] = CEKey::Key_F11;
	ScanCodeTable[0x058] = CEKey::Key_F12;
	ScanCodeTable[0x064] = CEKey::Key_F13;
	ScanCodeTable[0x065] = CEKey::Key_F14;
	ScanCodeTable[0x066] = CEKey::Key_F15;
	ScanCodeTable[0x067] = CEKey::Key_F16;
	ScanCodeTable[0x068] = CEKey::Key_F17;
	ScanCodeTable[0x069] = CEKey::Key_F18;
	ScanCodeTable[0x06A] = CEKey::Key_F19;
	ScanCodeTable[0x06B] = CEKey::Key_F20;
	ScanCodeTable[0x06C] = CEKey::Key_F21;
	ScanCodeTable[0x06D] = CEKey::Key_F22;
	ScanCodeTable[0x06E] = CEKey::Key_F23;
	ScanCodeTable[0x076] = CEKey::Key_F24;
}

void CEWindowsInputManager::CreateNumPadScanCodeTableKeys() {
	ScanCodeTable[0x052] = CEKey::Key_Keypad0;
	ScanCodeTable[0x04F] = CEKey::Key_Keypad1;
	ScanCodeTable[0x050] = CEKey::Key_Keypad2;
	ScanCodeTable[0x051] = CEKey::Key_Keypad3;
	ScanCodeTable[0x04B] = CEKey::Key_Keypad4;
	ScanCodeTable[0x04C] = CEKey::Key_Keypad5;
	ScanCodeTable[0x04D] = CEKey::Key_Keypad6;
	ScanCodeTable[0x047] = CEKey::Key_Keypad7;
	ScanCodeTable[0x048] = CEKey::Key_Keypad8;
	ScanCodeTable[0x049] = CEKey::Key_Keypad9;
	ScanCodeTable[0x04E] = CEKey::Key_KeypadAdd;
	ScanCodeTable[0x053] = CEKey::Key_KeypadDecimal;
	ScanCodeTable[0x135] = CEKey::Key_KeypadDivide;
	ScanCodeTable[0x11C] = CEKey::Key_KeypadEnter;
	ScanCodeTable[0x059] = CEKey::Key_KeypadEqual;
	ScanCodeTable[0x037] = CEKey::Key_KeypadMultiply;
	ScanCodeTable[0x04A] = CEKey::Key_KeypadSubtract;
}

void CEWindowsInputManager::CreateActionScanCodeTableKeys() {
	ScanCodeTable[0x02A] = CEKey::Key_LeftShift;
	ScanCodeTable[0x036] = CEKey::Key_RightShift;
	ScanCodeTable[0x01D] = CEKey::Key_LeftControl;
	ScanCodeTable[0x11D] = CEKey::Key_RightControl;
	ScanCodeTable[0x038] = CEKey::Key_LeftAlt;
	ScanCodeTable[0x138] = CEKey::Key_RightAlt;
	ScanCodeTable[0x15B] = CEKey::Key_LeftSuper;
	ScanCodeTable[0x15C] = CEKey::Key_RightSuper;
	ScanCodeTable[0x15D] = CEKey::Key_Menu;
	ScanCodeTable[0x039] = CEKey::Key_Space;
	ScanCodeTable[0x028] = CEKey::Key_Apostrophe;
	ScanCodeTable[0x033] = CEKey::Key_Comma;
	ScanCodeTable[0x00C] = CEKey::Key_Minus;
	ScanCodeTable[0x034] = CEKey::Key_Period;
	ScanCodeTable[0x035] = CEKey::Key_Slash;
	ScanCodeTable[0x027] = CEKey::Key_Semicolon;
	ScanCodeTable[0x00D] = CEKey::Key_Equal;
	ScanCodeTable[0x01A] = CEKey::Key_LeftBracket;
	ScanCodeTable[0x02B] = CEKey::Key_Backslash;
	ScanCodeTable[0x01B] = CEKey::Key_RightBracket;
	ScanCodeTable[0x029] = CEKey::Key_GraveAccent;
	ScanCodeTable[0x056] = CEKey::Key_World2;
	ScanCodeTable[0x001] = CEKey::Key_Escape;
	ScanCodeTable[0x01C] = CEKey::Key_Enter;
	ScanCodeTable[0x00F] = CEKey::Key_Tab;
	ScanCodeTable[0x00E] = CEKey::Key_Backspace;
	ScanCodeTable[0x152] = CEKey::Key_Insert;
	ScanCodeTable[0x153] = CEKey::Key_Delete;
	ScanCodeTable[0x14D] = CEKey::Key_Right;
	ScanCodeTable[0x14B] = CEKey::Key_Left;
	ScanCodeTable[0x150] = CEKey::Key_Down;
	ScanCodeTable[0x148] = CEKey::Key_Up;
	ScanCodeTable[0x149] = CEKey::Key_PageUp;
	ScanCodeTable[0x151] = CEKey::Key_PageDown;
	ScanCodeTable[0x147] = CEKey::Key_Home;
	ScanCodeTable[0x14F] = CEKey::Key_End;
	ScanCodeTable[0x03A] = CEKey::Key_CapsLock;
	ScanCodeTable[0x046] = CEKey::Key_ScrollLock;
	ScanCodeTable[0x145] = CEKey::Key_NumLock;
	ScanCodeTable[0x137] = CEKey::Key_PrintScreen;
	ScanCodeTable[0x146] = CEKey::Key_Pause;
}