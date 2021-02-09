#pragma once
#include <memory>

#include "CESubObject.h"

namespace Concept::GraphicsEngine::Direct3D {
	class CEDevice;

	class CEShaderConfig : public CESubObject {
	public:
	protected:
		friend class CEDevice;
		friend class std::default_delete<CEShaderConfig>;

		CEShaderConfig(uint32_t maxAttributeSizeInBytes, uint32_t maxPayloadSizeInBytes);

	private:
		D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
	};
}
