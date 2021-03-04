#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12ShapesPlayground final : public CEDX12Playground {
	public:
		CEDX12ShapesPlayground();

		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;

		void OnMouseDown(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseUp(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseMove(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnKeyUp(ConceptEngineFramework::Game::KeyCode key, char keyChar, const CETimer& gt) override;
		void OnKeyDown(ConceptEngineFramework::Game::KeyCode key, char keyChar, const CETimer& gt) override;
		void OnMouseWheel(ConceptEngineFramework::Game::KeyCode key, float wheelDelta, int x, int y) override;

		void UpdateCamera(const CETimer& gt) override;
		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;

	private:
		void BuildShapesGeometry();
		void BuildRenderItems();
		void BuildFrameResources();
		void BuildDescriptorHeaps();
		void BuildConstantBufferViews();
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
			std::vector<Resources::RenderItem*>& ritems) const;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;

		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> mGeometries;

		// List of all the render items.
		std::vector<std::unique_ptr<Resources::RenderItem>> mAllRitems;

		// Render items divided by PSO.
		std::vector<Resources::RenderItem*> mOpaqueRitems;

		Resources::PassConstants mMainPassCB;

		UINT mPassCbvOffset = 0;
		
		XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;
	};
}
