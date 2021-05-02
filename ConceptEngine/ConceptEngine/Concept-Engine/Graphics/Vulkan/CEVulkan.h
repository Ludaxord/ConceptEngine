#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::Vulkan {
	class CEVulkan : public Main::CEGraphics {
	public:
		CEVulkan();
		virtual ~CEVulkan();
		
		void Create() override;
		void CreateManagers() override;
	};
}
