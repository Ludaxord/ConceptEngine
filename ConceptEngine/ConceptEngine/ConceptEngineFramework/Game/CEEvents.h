#pragma once

#include <string>

#include "CEKeyCodes.h"
#include "Libraries/signals.hpp"

namespace ConceptEngineFramework::Game {
	/*
	 * Delegate holds function callbacks
	 */

	 /*
	  * Primary delegate template
	  */
	template <typename Func>
	class Delegate;

	template <typename R, typename... Args>
	class Delegate<R(Args ...)> {
	public:
		using slot = sig::slot<R(Args ...)>;
		using signal = sig::signal<R(Args ...)>;
		using connection = sig::connection;
		using scoped_connection = sig::scoped_connection;

		/*
		 * Add function callback to delegate
		 *
		 * @param f, function to add to delegate.
		 * @returns connection object that can be used to remove callback from delegate
		 */
		template <typename Func>
		connection operator+=(Func&& f) {
			return m_callbacks.connect(std::forward<Func>(f));
		}

		/*
		 * Remove callback function from delegate.
		 *
		 * @param f, function to remove delegate.
		 * @returns number of callback functions removed
		 */
		template <typename Func>
		std::size_t operator-=(Func&& f) {
			return m_callbacks.disconnect(std::forward<Func>(f));
		}

		/*
		 * Invoke delegate.
		 */
		opt::optional<R> operator()(Args ... args) {
			return m_callbacks(std::forward<Args>(args)...);
		}

	protected:
	private:
		signal m_callbacks;
	};

	/*
	 * Base class for all event args
	 */
	class EventArgs {
	public:
		EventArgs() = default;
	};

	/*
	 * Define default event
	 */
	using Event = Delegate<void(EventArgs&)>;

	/*
	 * Update event args.
	 */
	class UpdateEventArgs : public EventArgs {
	public:
		UpdateEventArgs(double deltaTime, double totalTime) : DeltaTime(deltaTime), TotalTime(totalTime) {

		}

		/*
		 * Elapsed time (in seconds)
		 */
		double DeltaTime;
		/*
		 * Total time application has been running (in seconds)
		 */
		double TotalTime;
	protected:
	private:
	};

	using UpdateEvent = Delegate<void(UpdateEventArgs&)>;

	class DPIScaleEventArgs : public EventArgs {
	public:
		typedef EventArgs base;

		DPIScaleEventArgs(float dpiScale) : DPIScale(dpiScale) {

		}

		float DPIScale;
	protected:
	private:
	};

	using DPIScaleEvent = Delegate<void(DPIScaleEventArgs&)>;

	/*
	 * EventArgs for WindowClosing event.
	 */
	class WindowCloseEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		WindowCloseEventArgs() : base(), ConfirmClose(true) {

		}

		/*
		 * User can cancel window closing operation by registering for Window::Close event on Window and setting
		 * WindowCloseEventArgs::ConfirmClose property to false if window should be kept open (for example, if closing window would cause unsaved file changes to be lost).
		 */
		bool ConfirmClose;

	protected:
	private:
	};

	using WindowCloseEvent = Delegate<void(WindowCloseEventArgs&)>;

	enum class KeyState {
		Released = 0,
		Pressed = 1
	};

	/*
	 * KeyEventArgs are used to keyboard key pressed/released events.
	 */
	class KeyEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		KeyEventArgs(KeyCode key, unsigned int c, KeyState state, bool control, bool shift, bool alt) :
			base(), Key(key), Char(c), State(state), Control(control), Shift(shift), Alt(alt) {

		}

		/*
		 * Key Code that was pressed or released.
		 */
		KeyCode Key;
		/**
		 * 32-bit character code that was pressed,
		 * This value will be 0 if non printable character was pressed
		 */
		unsigned int Char;

		/*
		 * Was key pressed or released
		 */
		KeyState State;
		/*
		 * Is control modifier pressed
		 */
		bool Control;
		/*
		 * Is shift modifier pressed
		 */
		bool Shift;
		/*
		 * Is Alt modifier pressed
		 */
		bool Alt;

	protected:
	private:
	};

	using KeyboardEvent = Delegate<void(KeyEventArgs&)>;

	/*
	 * MouseMotionEventArgs are uised to indicate mouse moved or was dragged over window.
	 */
	class MouseMotionEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		MouseMotionEventArgs(bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x,
			int y) : base(), LeftButton(leftButton), MiddleButton(middleButton),
			RightButton(rightButton), Control(control), Shift(shift), X(x), Y(y), RelX(0),
			RelY(0) {
		}

		/*
		* Is left mouse button pressed
		*/
		bool LeftButton;

		/*
		 * Is middle mouse button pressed
		 */
		bool MiddleButton;

		/*
		 * Is right mouse button pressed
		 */
		bool RightButton;

		/*
		 * Is Control pressed
		 */
		bool Control;

		/*
		 * Is Shift pressed
		 */
		bool Shift;

		/*
		 * X-position of cursor relative to upper-left corner of client area (in pixels)
		 */
		int X;

		/*
		 * Y-position of cursor relative to upper-left corner of client area (in pixels)
		 */
		int Y;

		/*
		 * How far mouse moved since last event (in pixels)
		 */
		int RelX;

		/*
		 * How far mouse moved since last event (in pixels)
		 */
		int RelY;

	protected:
	private:
	};

	using MouseMotionEvent = Delegate<void(MouseMotionEventArgs&)>;

	enum class MouseButton {
		None = 0,
		Left = 1,
		Right = 1,
		Middle = 3,
		Additional1 = 4,
		Additional2 = 5
	};

	enum class ButtonState {
		Released = 0,
		Pressed = 1
	};

	class MouseButtonEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		MouseButtonEventArgs(MouseButton button, ButtonState state, bool leftButton, bool middleButton,
			bool rightButton, bool control,
			bool shift, int x, int y) : base(), Button(button), State(state), LeftButton(leftButton),
			MiddleButton(middleButton), RightButton(rightButton),
			Control(control), Shift(shift), X(x), Y(y) {

		}

		/*
		 * Mouse button that was pressed or released
		 */
		MouseButton Button;
		/*
		 * Was button pressed or released
		 */
		ButtonState State;
		/*
		 * Is left mouse button pressed
		 */
		bool LeftButton;
		/*
		 * Is middle mouse button pressed
		 */
		bool MiddleButton;
		/*
		 * Is right mouse button pressed
		 */
		bool RightButton;
		/*
		 * Is Control key pressed
		 */
		bool Control;
		/*
		 * Is Shift key pressed
		 */
		bool Shift;

		/*
		 * X-position of cursor relative to upper-left corner of client area
		 */
		int X;

		/*
		 * Y-position of cursor relative to upper-left corner of client area
		 */
		int Y;
	protected:
	private:
	};

	using MouseButtonEvent = Delegate<void(MouseButtonEventArgs&)>;

	/*
	 * MouseWheelEventArgs indicates if mouse wheel was moved and how much.
	 */
	class MouseWheelEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		MouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control,
			bool shift, int x, int y) : base(), WheelDelta(wheelDelta), LeftButton(leftButton),
			MiddleButton(middleButton), RightButton(rightButton),
			Control(control), Shift(shift), X(x), Y(y) {

		}

		/*
		 * How much mouse wheel has moved, Positive value indicates that wheel was moved to right, Negative value indicates wheel was moved to left
		 */
		float WheelDelta;

		/*
		 * Is left mouse button pressed
		 */
		bool LeftButton;

		/*
		 * Is middle mouse button pressed
		 */
		bool MiddleButton;

		/*
		 * Is right mouse button pressed
		 */
		bool RightButton;

		/*
		 * Is Control key pressed
		 */
		bool Control;

		/*
		 * Is Shift key pressed
		 */
		bool Shift;

		/*
		 * X-position of cursor relative to upper-left corner of client area
		 */
		int X;

		/*
		 * Y-position of cursor relative to upper-left corner of client area
		 */
		int Y;
	protected:
	private:
	};

	using MouseWheelEvent = Delegate<void(MouseWheelEventArgs&)>;

	enum class WindowState {
		/*
		 * Window has been resized
		 */
		Restored = 0,
		/*
		 * Window has been minimized
		 */
		 Minimized = 1,
		 /*
		  * Window has been maximized
		  */
		  Maximized = 2,
	};

	/*
	 * Event args to indicate window has been resized.
	 */
	class ResizeEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		ResizeEventArgs(int width, int height, WindowState state) : base(), Width(width), Height(height), State(state) {

		}

		/*
		 * New Width of window
		 */
		int Width;

		/*
		 * New Height of window
		 */
		int Height;

		/*
		 * State of window (e.x. Minimized, Maximized etc.)
		 */
		WindowState State;

	protected:
	private:
	};

	using ResizeEvent = Delegate<void(ResizeEventArgs&)>;

	/*
	 * Generic user event args
	 */
	class UserEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		UserEventArgs(int code, void* data1, void* data2) : base(), Code(code), Data1(data1), Data2(data2) {

		}

		int Code;
		void* Data1;
		void* Data2;
	protected:
	private:
	};

	using UserEvent = Delegate<void(UserEventArgs&)>;

	/*
	 * Used to notify runtime error occurred.
	 */
	class RuntimeErrorEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		RuntimeErrorEventArgs(const std::string& errorString, const std::string& compilerError) :
			base(), ErrorString(errorString), CompilerError(compilerError) {
		}

		std::string ErrorString;
		std::string CompilerError;

	protected:
	private:
	};

	using RuntimeErrorEvent = Delegate<void(RuntimeErrorEventArgs&)>;

	enum class FileAction {
		/*
		 * Unknown action triggered event
		 */
		Unknown,
		/*
		 * File was added to directory.
		 */
		 Added,
		 /*
		  * File was removed from directory.
		  */
		  Removed,
		  /*
		   * File was modified. This might indicate file's timestamp was modified or another attribute was modified.
		   */
		   Modified,
		   /*
			* File was renamed and event stores previous name
			*/
			RenameOld,
			/*
			 * File was renamed and event stores new name
			 */
			 RenameNew
	};

	class FileChangedEventArgs : public EventArgs {
	public:
		using base = EventArgs;

		FileChangedEventArgs(FileAction action, const std::wstring& path) : base(), Action(action), Path(path) {
		}

		/*
		 * Action that triggered event
		 */
		FileAction Action;

		/*
		 * File or directory path that was modified
		 */
		std::wstring Path;

	protected:
	private:
	};

	using FileChangeEvent = Delegate<void(FileChangedEventArgs&)>;
}
