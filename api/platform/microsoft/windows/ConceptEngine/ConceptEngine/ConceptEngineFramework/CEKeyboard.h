#pragma once
#include <bitset>
#include <queue>

class CEKeyboard {
	friend class CEWindow;

public:
	class CEKeyboardEvent {
	public:
		enum class CEKeyboardEventType {
			Press,
			Release,
			Invalid
		};

	private:
		CEKeyboardEventType type;
		unsigned char code;
	public:
		CEKeyboardEvent() : type(CEKeyboardEventType::Invalid), code(0u) {

		}

		CEKeyboardEvent(CEKeyboardEventType type, unsigned char code) noexcept : type(type), code(code) {

		}

		bool IsPressed() const noexcept {
			return type == CEKeyboardEventType::Press;
		}

		bool IsValid() const noexcept {
			return type != CEKeyboardEventType::Invalid;
		}

		bool IsReleased() const noexcept {
			return type == CEKeyboardEventType::Release;
		}

		unsigned char GetCode() const noexcept {
			return code;
		}
	};

public:
	CEKeyboard() = default;
	CEKeyboard(const CEKeyboard&) = delete;
	CEKeyboard& operator=(const CEKeyboard&) = delete;
	bool IsKeyPressed(unsigned char keyCode) const noexcept;
	CEKeyboardEvent ReadKey() noexcept;
	bool IsKeyEmpty() const noexcept;
	void ClearKey() noexcept;

	char ReadChar() noexcept;
	bool IsCharEmpty() noexcept;
	void ClearChar() noexcept;
	void Clear() noexcept;

	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;
private:
	//keyCodes as chars
	void OnKeyPressed(unsigned char keyCode) noexcept;
	void OnKeyReleased(unsigned char keyCode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template <typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int numberOfKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autoRepeatEnabled = false;
	std::bitset<numberOfKeys> keyStates;
	std::queue<CEKeyboardEvent> keyBuffer;
	std::queue<char> charBuffer;
};
