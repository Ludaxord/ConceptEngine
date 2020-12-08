#include "CEKeyboard.h"

bool CEKeyboard::IsKeyPressed(unsigned char keyCode) const noexcept {
	return keyStates[keyCode];
}

CEKeyboard::CEKeyboardEvent CEKeyboard::ReadKey() noexcept {
	if (!keyBuffer.empty()) {
		CEKeyboardEvent e = keyBuffer.front();
		keyBuffer.pop();
		return e;
	}

	return CEKeyboardEvent();
}

bool CEKeyboard::IsKeyEmpty() const noexcept {
	return keyBuffer.empty();
}

void CEKeyboard::ClearKey() noexcept {
	keyBuffer = std::queue<CEKeyboardEvent>();
}

char CEKeyboard::ReadChar() noexcept {
	if (!charBuffer.empty()) {
		unsigned char charCode = charBuffer.front();
		charBuffer.pop();
		return charCode;
	}

	return 0;
}

bool CEKeyboard::IsCharEmpty() noexcept {
	return charBuffer.empty();
}

void CEKeyboard::ClearChar() noexcept {
	charBuffer = std::queue<char>();
}

void CEKeyboard::Clear() noexcept {
	ClearKey();
	ClearChar();
}

void CEKeyboard::EnableAutoRepeat() noexcept {
	autoRepeatEnabled = true;
}

void CEKeyboard::DisableAutoRepeat() noexcept {
	autoRepeatEnabled = false;
}

bool CEKeyboard::IsAutoRepeatEnabled() const noexcept {
	return autoRepeatEnabled;
}

void CEKeyboard::OnKeyPressed(unsigned char keyCode) noexcept {
	keyStates[keyCode] = true;
	const auto keyboardEvent = CEKeyboardEvent(CEKeyboardEvent::CEKeyboardEventType::Press, keyCode);
	keyBuffer.push(keyboardEvent);
	TrimBuffer(keyBuffer);
}

void CEKeyboard::OnKeyReleased(unsigned char keyCode) noexcept {
	keyStates[keyCode] = false;
	const auto keyboardEvent = CEKeyboardEvent(CEKeyboardEvent::CEKeyboardEventType::Release, keyCode);
	keyBuffer.push(keyboardEvent);
	TrimBuffer(keyBuffer);
}

void CEKeyboard::OnChar(char character) noexcept {
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void CEKeyboard::ClearState() noexcept {
	keyStates.reset();
}

template <typename T>
void CEKeyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}
