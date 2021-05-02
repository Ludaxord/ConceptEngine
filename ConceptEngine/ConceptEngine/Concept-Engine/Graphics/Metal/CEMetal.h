#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Metal {
	class CEMetal : public Main::CEGraphics {
	public:
		CEMetal();
		virtual ~CEMetal();
		
		void Create() override;
		void CreateManagers() override;
	};
}
