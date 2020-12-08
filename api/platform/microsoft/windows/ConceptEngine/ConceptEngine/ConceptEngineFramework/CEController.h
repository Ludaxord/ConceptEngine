#pragma once
//TODO: Check documentation to implement controller actions
class CEController {
	friend class CEWindow;

public:
	enum class CEControllerEventType {
		Press,
		Release,
		MoveLeftStick,
		MoveRightStick,
		MoveController,
		Invalid
	};

};

