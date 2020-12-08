#include "CEMouse.h"
#include <Windows.h>

std::pair<int, int> CEMouse::GetMousePosition() const noexcept {
	return {x, y};
}

int CEMouse::GetMousePositionX() const noexcept {
	return x;
}

int CEMouse::GetMousePositionY() const noexcept {
	return y;
}

bool CEMouse::IsMouseInWindow() const noexcept {
	return isMouseInWindow;
}

bool CEMouse::IsLeftKeyPressed() const noexcept {
	return isLeftKeyPressed;
}

bool CEMouse::IsRightKeyPressed() const noexcept {
	return isRightKeyPressed;
}

CEMouse::CEMouseEvent CEMouse::Read() noexcept {
	if (!buffer.empty()) {
		CEMouseEvent e = buffer.front();
		buffer.pop();
		return e;
	}

	return CEMouseEvent();
}

void CEMouse::Clear() noexcept {
	buffer = std::queue<CEMouseEvent>();
}

void CEMouse::OnMouseMove(int moveX, int moveY) noexcept {
	x = moveX;
	y = moveY;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::move, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnMouseEnter() noexcept {
	isMouseInWindow = true;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::enter, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnMouseLeave() noexcept {
	isMouseInWindow = false;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::leave, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnLeftKeyPressed(int moveX, int moveY) noexcept {
	isLeftKeyPressed = true;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::leftKeyPress, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnRightKeyPressed(int moveX, int moveY) noexcept {
	isRightKeyPressed = true;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::rightKeyPress, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnLeftKeyReleased(int moveX, int moveY) noexcept {
	isLeftKeyPressed = false;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::leftKeyRelease, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnRightKeyReleased(int moveX, int moveY) noexcept {
	isRightKeyPressed = false;
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::rightKeyRelease, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnWheelUp(int moveX, int moveY) noexcept {
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::wheelUp, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::OnWheelDown(int moveX, int moveY) noexcept {
	CEMouseEvent e = CEMouseEvent(CEMouseEvent::CEMouseEventType::wheelDown, *this);
	buffer.push(e);
	TrimBuffer();
}

void CEMouse::TrimBuffer() noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}

void CEMouse::OnWheelDelta(int moveX, int moveY, int delta) noexcept {
	wheelDeltaCarry += delta;
	while (wheelDeltaCarry >= WHEEL_DELTA) {
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(moveX, moveY);
	}
	while (wheelDeltaCarry <= WHEEL_DELTA) {
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(moveX, moveY);
	}
}
