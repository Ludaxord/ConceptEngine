#pragma once
#include "CEAABB.h"

namespace ConceptEngine::Render::Scene {
	class CEFrustum {
	public:
		CEFrustum() = default;
		CEFrustum(float screenDepth, const CEMatrixFloat4X4& view, const CEMatrixFloat4X4& projection);

		void Create(float screenDepth, const CEMatrixFloat4X4& view, const CEMatrixFloat4X4& projection);

#ifdef WINDOWS_PLATFORM
		
		CEFrustum(float screenDepth, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

		void Create(float screenDepth, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
#endif

		bool CheckAABB(const CEAABB& boundingBox);

	private:
		CEVectorFloat4 Planes[6];
	};
}
