#pragma once
#include "../CESave.h"

namespace ConceptEngine::Save::Game {
	class CEGameSave : public CESave {
	public:
		friend class CEGameSaveReader;
		friend class CEGameSaveWriter;
	};
}
