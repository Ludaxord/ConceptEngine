#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Metal {
	class CEMetal : public Main::CEGraphics {
	public:
		CEMetal();
		~CEMetal() override;
		
		void Create() override;
		void CreateManagers() override;

		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
		void CreateGraphicsManager() override;
	};
}
