#pragma once

#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12InitDirect3DPlayground final : public ConceptEngineFramework::Graphics::DirectX12::CEDX12Playground {

	public:
		CEDX12InitDirect3DPlayground();
		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
	};
}
