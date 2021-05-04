#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Vulkan {
	class CEVulkan final : public Main::CEGraphics {
	public:
		CEVulkan();
		~CEVulkan() override;
		
		bool Create() override;
		bool CreateManagers() override;
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	};
}
