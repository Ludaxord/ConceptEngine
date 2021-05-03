#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Vulkan {
	class CEVulkan final : public Main::CEGraphics {
	public:
		CEVulkan();
		~CEVulkan() override;
		
		void Create() override;
		void CreateManagers() override;
		
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
		void CreateGraphicsManager() override;
	};
}
