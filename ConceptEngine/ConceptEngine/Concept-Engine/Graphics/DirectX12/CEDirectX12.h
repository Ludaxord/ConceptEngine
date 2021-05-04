#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::DirectX12 {
	class CEDirectX12 final : public Main::CEGraphics {
	public:
		CEDirectX12();
		~CEDirectX12() override;

		bool CreateManagers() override;
		bool CreateGraphicsManager() override;
		bool CreateTextureManager() override;
		
		bool Create() override;
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	};
}
