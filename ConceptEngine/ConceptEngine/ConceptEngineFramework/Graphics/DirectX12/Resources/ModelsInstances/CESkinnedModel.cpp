#include "CESkinnedModel.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources::ModelsInstances;

void CESkinnedModel::UpdateAnimation(float dt) {
	TimePos += dt;

	//Loop animation
	if (TimePos > ModelInfo->GetClipEndTime(ClipName))
		TimePos = 0.0f;

	//Compute final transforms for this time position
	ModelInfo->GetFinalTransform(ClipName, TimePos, FinalTransforms);
}
