#pragma once

#include "../Main/CEGraphics.h"
#include "../../Time/CETimestamp.h"

namespace ConceptEngine::Graphics::DirectX12 {
	class CEDirectX12 final : public Main::CEGraphics {
	public:
		CEDirectX12();
		~CEDirectX12() override;

		bool CreateManagers() override;

		bool Create() override;
		void Update(Time::CETimestamp DeltaTime) override;
		void Render() override;
		void Resize() override;
		void Destroy() override;

	private:
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		bool CreateRendererManager() override;
		bool CreateDebugUi() override;
		bool CreateShaderCompiler() override;
	};
}
