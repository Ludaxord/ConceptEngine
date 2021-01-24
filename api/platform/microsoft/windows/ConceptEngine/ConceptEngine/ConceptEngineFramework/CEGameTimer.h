#pragma once
#include <memory>

// namespace Concept::GameEngine {
	class CEGameTimer {
	public:
		CEGameTimer();
		~CEGameTimer();

		/*
		 * Tick high resolution timer.
		 */
		void Tick();

		/*
		 * Reset elapsed and total time.
		 *
		 */
		void Reset();

		/*
		 * Get elapsed time between ticks
		 */
		double ElapsedSeconds() const;
		double ElapsedMilliseconds() const;
		double ElapsedMicroseconds() const;
		double ElapsedNanoseconds() const;

		/*
		 * Get total time since timer was started (or reset).
		 */
		double TotalSeconds() const;
		double TotalMilliseconds() const;
		double TotalMicroseconds() const;
		double TotalNanoseconds() const;
	protected:
	private:
		class impl;
		std::unique_ptr<impl> pImpl;

	};
// }
