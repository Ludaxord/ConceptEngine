#pragma once

namespace ConceptEngine::Core::Platform::Windows::Threading {
	class CEWindowsMutex {
		friend class CEWindowsConditionVariable;
	public:
		CEWindowsMutex() noexcept : Section() {
			InitializeCriticalSection(&Section);
		}

		~CEWindowsMutex() {
			DeleteCriticalSection(&Section);
		}

		void Lock() noexcept {
			EnterCriticalSection(&Section);
		}

		bool TryLock() noexcept {
			return TryEnterCriticalSection(&Section);
		}

		void Unlock() noexcept {
			LeaveCriticalSection(&Section);
		}

	private:
		CRITICAL_SECTION Section;
	};
}
