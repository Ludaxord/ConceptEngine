#pragma once
#include <queue>
#include <utility>

class CEMouse {
	friend class CEWindow;

public:
	class CEMouseEvent {
	public:
		enum class CEMouseEventType {
			LeftKeyPress,
			LeftKeyRelease,
			RightKeyPress,
			RightKeyRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};

	private:
		CEMouseEventType type;
		bool isLeftKeyPressed;
		bool isRightKeyPressed;
		int x;
		int y;

	public:
		CEMouseEvent() : type(CEMouseEventType::Invalid), isLeftKeyPressed(false), isRightKeyPressed(false), x(0),
		                 y(0) {

		}

		CEMouseEvent(CEMouseEventType type, const CEMouse& parent) :
			type(type),
			isLeftKeyPressed(parent.isLeftKeyPressed),
			isRightKeyPressed(parent.isRightKeyPressed),
			x(parent.x),
			y(parent.y) {
		}

		CEMouseEvent(CEMouseEventType type, bool isLeftKeyPressed, bool isRightKeyPressed, int x, int y) :
			type(type),
			isLeftKeyPressed(isLeftKeyPressed),
			isRightKeyPressed(isRightKeyPressed),
			x(x),
			y(y) {
		}

		bool IsValid() const noexcept {
			return type != CEMouseEventType::Invalid;
		}

		CEMouseEventType GetType() const noexcept {
			return type;
		}

		std::pair<int, int> GetMousePosition() const noexcept {
			return {x, y};
		}

		int GetMousePositionX() const noexcept {
			return x;
		}

		int GetMousePositionY() const noexcept {
			return y;
		}

		bool IsLeftKeyPressed() const noexcept {
			return isLeftKeyPressed;
		}

		bool IsRightKeyPressed() const noexcept {
			return isRightKeyPressed;
		}
	};

public:
	CEMouse() = default;
	CEMouse(const CEMouse&) = delete;
	CEMouse& operator=(const CEMouse&) = delete;
	std::pair<int, int> GetMousePosition() const noexcept;
	int GetMousePositionX() const noexcept;
	int GetMousePositionY() const noexcept;
	bool IsMouseInWindow() const noexcept;
	bool IsLeftKeyPressed() const noexcept;
	bool IsRightKeyPressed() const noexcept;
	CEMouseEvent Read() noexcept;

	bool IsEmpty() const noexcept {
		return buffer.empty();
	}

	void Clear() noexcept;
private:
	void OnMouseMove(int moveX, int moveY) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void OnLeftKeyPressed(int moveX, int moveY) noexcept;
	void OnRightKeyPressed(int moveX, int moveY) noexcept;
	void OnLeftKeyReleased(int moveX, int moveY) noexcept;
	void OnRightKeyReleased(int moveX, int moveY) noexcept;
	void OnWheelUp(int moveX, int moveY) noexcept;
	void OnWheelDown(int moveX, int moveY) noexcept;
	void TrimBuffer() noexcept;
	void OnWheelDelta(int moveX, int moveY, int delta) noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	int x;
	int y;
	bool isMouseInWindow = false;
	bool isLeftKeyPressed = false;
	bool isRightKeyPressed = false;
	int wheelDeltaCarry = 0;
	std::queue<CEMouseEvent> buffer;
};
