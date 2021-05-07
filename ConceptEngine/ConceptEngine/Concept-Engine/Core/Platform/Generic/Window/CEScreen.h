#pragma once

#include "../../../../Utilities/CEScreenUtilities.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	class CEScreen {
	public:
		friend class CEPlatform;
		CEScreen();
		virtual ~CEScreen();

		virtual bool Create() = 0;
	};
}
