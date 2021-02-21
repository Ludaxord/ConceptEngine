#pragma once

#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12BoxPlayground final : public ConceptEngineFramework::Graphics::DirectX12::CEDX12Playground {

	public:
		CEDX12BoxPlayground();
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
	};
}
