#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CETypes.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEUploadBuffer.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12BoxPlayground final : public CEDX12Playground {

	public:
		CEDX12BoxPlayground();
		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;

		void OnMouseDown(KeyCode key, int x, int y) override;
		void OnMouseUp(KeyCode key, int x, int y) override;
		void OnMouseMove(KeyCode key, int x, int y) override;
		void OnKeyUp(KeyCode key, char keyChar) override;
		void OnKeyDown(KeyCode key, char keyChar) override;
		void OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) override;

	protected:
		void BuildShadersAndInputLayout() override;
	
	private:
		void BuildBoxGeometry();
		void BuildPSO();

	private:
		std::unique_ptr<Resources::CEUploadBuffer<Resources::CEObjectConstants>> mObjectCB = nullptr;

		std::unique_ptr<Resources::MeshGeometry> m_boxGeo = nullptr;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;

		Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode;
		Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		
	};
}
