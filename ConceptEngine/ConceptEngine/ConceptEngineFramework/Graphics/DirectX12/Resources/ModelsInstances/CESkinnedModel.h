#pragma once
#include "../CEModel.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources::ModelsInstances {
	struct CESkinnedModel : public CEModel {

		void UpdateAnimation(float dt) override;
	};
}
