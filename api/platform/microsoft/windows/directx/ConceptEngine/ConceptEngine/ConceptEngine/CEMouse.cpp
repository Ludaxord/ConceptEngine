#include "CEMouse.h"

std::pair<int, int> CEMouse::GetMousePosition() const noexcept {
	return {x, y};
}

int CEMouse::GetMousePositionX() const noexcept {
	return x;
}

int CEMouse::GetMousePositionY() const noexcept {
	return y;
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
