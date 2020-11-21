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
	
}

void CEMouse::OnLeftKeyPressed(int moveX, int moveY) noexcept {
}

void CEMouse::OnRightKeyPressed(int moveX, int moveY) noexcept {
}

void CEMouse::OnLeftKeyReleased(int moveX, int moveY) noexcept {
}

void CEMouse::OnRightKeyReleased(int moveX, int moveY) noexcept {
}

void CEMouse::OnWheelUp(int moveX, int moveY) noexcept {
}

void CEMouse::OnWheelDown(int moveX, int moveY) noexcept {
}

void CEMouse::TrimBuffer() noexcept {
}
