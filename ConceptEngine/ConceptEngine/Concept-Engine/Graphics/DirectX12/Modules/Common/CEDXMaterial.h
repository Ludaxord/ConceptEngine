#pragma once

#include "../../../Main/Common/CEMaterial.h"

namespace ConceptEngine::Graphics::DirectX12::Modules::Common {
	class CEDXMaterial : public Main::Common::CEMaterial {
	public:
		CEDXMaterial(const Main::Common::CEMaterialProperties& Properties)
			: CEMaterial(Properties) {
		}

		void SetAlbedo(float R, float G, float B) override;
		void SetAlbedo(const DirectX::XMFLOAT3& albedo);
	};
}
