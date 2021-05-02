#pragma once

#include "../CESave.h"

namespace ConceptEngine::Save::Project {
	class CEProjectSave : public CESave {
	public:
		friend class CEProjectSaveReader;
		friend class CEProjectSaveWriter;
	};
}
