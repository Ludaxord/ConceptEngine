#pragma once

#include "../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CETypes.h"
#include "../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEUploadBuffer.h"

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12BoxPlayground final : public ConceptEngineFramework::Graphics::DirectX12::CEDX12Playground {

	public:
		CEDX12BoxPlayground();
		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;

	private:
		void BuildShadersAndInputLayout();
		void BuildBoxGeometry();
		void BuildPSO();

		std::unique_ptr<CEUploadBuffer<CEObjectConstants>> mObjectCB = nullptr;

		std::unique_ptr<MeshGeometry> m_boxGeo = nullptr;
		
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;

		Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode;
		Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
	};
}
