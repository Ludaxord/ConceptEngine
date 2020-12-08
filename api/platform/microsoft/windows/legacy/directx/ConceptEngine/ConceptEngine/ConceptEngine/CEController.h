#pragma once
//Abstract class for controller => to extend CEPlaystationController and CEXboxController
//TODO: Check documentation to implement controller actions
class CEController {
	friend class CEWindow;

public:
	enum class CEControllerEventType {
		press,
		release,
		moveLeftStick,
		moveRightStick,
		moveController,
		invalid
	};

};
