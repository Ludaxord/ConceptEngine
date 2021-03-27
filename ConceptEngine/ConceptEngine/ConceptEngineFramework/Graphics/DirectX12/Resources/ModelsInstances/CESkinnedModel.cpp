#include "CESkinnedModel.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources::ModelsInstances;

void CESkinnedModel::UpdateAnimation(float dt) {
	m_timePos += dt;

	//Loop animation
	if (m_timePos > m_modelInfo->GetClipEndTime(m_clipName))
		m_timePos = 0.0f;

	//Compute final transforms for this time position
	m_modelInfo->GetFinalTransform(m_clipName, m_timePos, m_finalTransforms);
}
