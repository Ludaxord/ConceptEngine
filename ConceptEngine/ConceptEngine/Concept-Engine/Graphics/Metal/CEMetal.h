#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Metal {
	class CEMetal : public Main::CEGraphics {
	public:
		CEMetal();
		~CEMetal() override;
		
		bool Create() override;
		bool CreateManagers() override;

		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	private:
		bool CreateRendererManager() override;
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
	};
}
