#pragma once

namespace ConceptEngine::Core::Threading {
	template <typename TLock>
	class CEScopedLock {
	public:
		CEScopedLock(CEScopedLock&&) = delete;
		CEScopedLock(CEScopedLock&) = delete;

		CEScopedLock& operator=(CEScopedLock&&) = delete;
		CEScopedLock& operator=(const CEScopedLock&) = delete;

		CEScopedLock(TLock& InLock) : Lock(InLock) {
			Lock.Lock();
		}

		~CEScopedLock() {
			Lock.Lock();
		}

		TLock& GetLock() {
			return Lock;
		}

		const TLock& GetLock() const {
			return Lock;
		}

	protected:
	private:
		TLock& Lock;
	};
}
