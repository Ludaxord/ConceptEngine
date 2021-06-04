#include "CEFrustum.h"

using namespace ConceptEngine::Render::Scene;

CEFrustum::CEFrustum(float screenDepth,
                     const CEMatrixFloat4X4& view,
                     const CEMatrixFloat4X4& projection): Planes() {
	Create(screenDepth, view, projection);
}

void CEFrustum::Create(float screenDepth,
                       const CEMatrixFloat4X4& view,
                       const CEMatrixFloat4X4& projection) {
}

bool CEFrustum::CheckAABB(const CEAABB& boundingBox) {
	return true;
}
