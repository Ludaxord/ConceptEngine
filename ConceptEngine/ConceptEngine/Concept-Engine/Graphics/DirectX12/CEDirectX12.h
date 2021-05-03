#pragma once

#include "../Main/CEGraphics.h"

namespace ConceptEngine::Graphics::DirectX12 {
	class CEDirectX12 final : public Main::CEGraphics {
	public:
		CEDirectX12();
		~CEDirectX12() override;

		void CreateManagers() override;
		void CreateGraphicsManager() override;

		void Create() override;
		void Update() override;
		void Render() override;
		void Resize() override;
		void Destroy() override;
	};
}
