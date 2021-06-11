#pragma once

#include "../../../../Utilities/CEScreenUtilities.h"
#include "../../../Common/CERefCountedObject.h"

namespace ConceptEngine::Core::Platform::Generic::Window {
	class CEScreen : public CERefCountedObject {
	public:
		friend class CEPlatform;
		CEScreen();
		virtual ~CEScreen();

		virtual bool Create() = 0;
	};
}
