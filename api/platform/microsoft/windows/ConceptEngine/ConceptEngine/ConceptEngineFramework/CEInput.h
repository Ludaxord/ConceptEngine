#pragma once
#include <activation.h>
#include <intsafe.h>
#include <memory>
#include <gainput/gainput.h>

enum class CEMouseButton {
	None = 0,
	Left = 1,
	Right = 2,
	Middle = 3
};

enum class CEButtonState {
	Released = 0,
	Pressed = 1
};

class CEInput {
public:
	CEInput();

	gainput::DeviceId GetKeyboardId() {
		return keyboardDevice_;
	}

	gainput::DeviceId GetMouseId() {
		return mouseDevice_;
	}

	gainput::DeviceId GetControllerId(unsigned index) {
		assert(index >= 0 && index < gainput::MaxPadCount);
		return gamePadDevice_[index];
	}

	void ProcessInput() {
		inputManager_.Update();
	}

	std::shared_ptr<gainput::InputMap> CreateInputMap(const char* name) {
		return std::make_shared<gainput::InputMap>(inputManager_, name);
	}

	static CEMouseButton DecodeMouseButton(UINT messageId) {
		auto mouseButton = CEMouseButton::None;
		switch (messageId) {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK: {
			mouseButton = CEMouseButton::Left;
		}
		break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK: {
			mouseButton = CEMouseButton::Right;
		}
		break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK: {
			mouseButton = CEMouseButton::Middle;
		}
		break;
		}
		return mouseButton;
	}

private:
	/*
	 * Gainput variables:
	 */
	gainput::InputManager inputManager_;
	gainput::DeviceId keyboardDevice_;
	gainput::DeviceId mouseDevice_;
	gainput::DeviceId gamePadDevice_[gainput::MaxPadCount];

};
