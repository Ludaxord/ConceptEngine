#pragma once

#include <string>

#include "../Common/CERefCountedObject.h"

namespace ConceptEngine::Core::Threading {

	typedef void (*ThreadFunction)();
	typedef uint64 ThreadID;

	#define INVALID_THREAD_ID 0
	
	class CEThread : public Common::CERefCountedObject {
	public:
		virtual ~CEThread() = default;

		virtual void Wait() = 0;

		virtual void SetName(const std::string& name) = 0;

		virtual ThreadID GetID() = 0;

		static CEThread* Create(ThreadFunction Func);
	};
}
