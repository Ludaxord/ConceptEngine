#pragma once
#include <combaseapi.h>

namespace ConceptEngine::Core::Common {
	class CEManager {
	public:
		CEManager() {
		};
		virtual ~CEManager() = default;
		virtual bool Create() = 0;
	};
}
