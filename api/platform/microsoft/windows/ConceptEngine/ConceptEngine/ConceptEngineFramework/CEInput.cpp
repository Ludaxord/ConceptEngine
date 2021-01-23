#include "CEInput.h"

using namespace Concept::GameEngine;

CEInput::CEInput() {
	keyboardDevice_ = inputManager_.CreateDevice<gainput::InputDeviceKeyboard>();
	mouseDevice_ = inputManager_.CreateDevice<gainput::InputDeviceMouse>();
	for (unsigned i = 0; i < gainput::MaxPadCount; ++i) {
		gamePadDevice_[i] = inputManager_.CreateDevice<gainput::InputDevicePad>(i);
	}
	inputManager_.SetDisplaySize(1, 1);
}
