#pragma once

#include "../Main/CEGraphics.h"
#include "../../Time/CETimestamp.h"

namespace ConceptEngine::Graphics::Vulkan {
	class CEVulkan final : public Main::CEGraphics {
	public:
		CEVulkan();
		~CEVulkan() override;
		
		bool Create() override;
		bool CreateManagers() override;
		
		void Update(Time::CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	private:
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		bool CreateRendererManager() override;
		bool CreateDebugUi() override;
		bool CreateShaderCompiler() override;
		bool CreateMeshManager() override;
	};
}
