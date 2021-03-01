#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12LitShapesPlayground final : public CEDX12Playground {
	public:
		CEDX12LitShapesPlayground();

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

		void UpdateCamera(const CETimer& gt) override;
		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;

	private:
		void BuildGeometry();
		void BuildMaterials();
		void BuildRenderItems();
		void BuildFrameResources();
		
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
	};
}
