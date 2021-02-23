#pragma once
#include <DirectXMath.h>
#include <memory>

#include "../CEPlayground.h"
#include "Resources/CETypes.h"

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager;

	class CEDX12Playground : public CEPlayground {
	public:
		CEDX12Playground();

		virtual void Init(CEGraphicsManager* manager) override;
		virtual void Create() override;
		virtual void Update(const CETimer& gt) override;
		virtual void Render(const CETimer& gt) override;
	protected:
		CEDX12Manager* m_dx12manager = nullptr;

		XMFLOAT4X4 mWorld = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mView = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mProj = Resources::MatrixIdentity4X4();
	};
}
