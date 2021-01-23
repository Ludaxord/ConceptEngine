#pragma once

#include "signals.hpp"

namespace Concept::GameEngine {
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
	protected:

	private:

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

	};

	using UpdateEvent = Delegate<void(UpdateEventArgs&)>;

	class FileChangedEventArgs : public EventArgs {

	};

	using FileChangeEvent = Delegate<void(FileChangedEventArgs&)>;
}
