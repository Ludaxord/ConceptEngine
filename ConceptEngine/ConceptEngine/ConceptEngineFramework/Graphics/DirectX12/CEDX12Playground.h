#pragma once
#include <memory>

#include "../CEPlayground.h"

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager;

	class CEDX12Playground : public CEPlayground {
	public:
		CEDX12Playground();

		virtual void Init(CEGraphicsManager* manager) override;
		virtual void Update(const CETimer& gt) override;
		virtual void Render(const CETimer& gt) override;
	protected:
		CEDX12Manager* m_dx12manager = nullptr;
	};
}
