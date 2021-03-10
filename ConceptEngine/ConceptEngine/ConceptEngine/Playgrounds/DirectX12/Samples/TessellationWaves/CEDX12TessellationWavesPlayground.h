#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEBlurFilter.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEGpuWaves.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CERenderTarget.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CESobelFilter.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEWaves.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12TessellationWavesPlayground final : public CEDX12Playground {
	public:
		CEDX12TessellationWavesPlayground();

		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;

		void OnMouseDown(KeyCode key, int x, int y) override;
		void OnMouseUp(KeyCode key, int x, int y) override;
		void OnMouseMove(KeyCode key, int x, int y) override;
		void OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) override;
		void OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) override;
		void OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) override;

		void UpdateCamera(const CETimer& gt) override;
		void AnimateMaterials(const CETimer& gt) override;
		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMaterialCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;

	private:
		void LoadTextures();
		void BuildDescriptorHeaps();
		void BuildLandGeometry();
		void BuildMaterials();
		void BuildRenderItems();
		void BuildFrameResources();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
			std::vector<Resources::RenderItem*>& ritems) const;

		float GetHillsHeight(float x, float z) const;
		XMFLOAT3 GetHillsNormal(float x, float z) const;

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;

		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> mGeometries;
		std::unordered_map<std::string, std::unique_ptr<Resources::Material>> mMaterials;
		std::unordered_map<std::string, std::shared_ptr<Resources::Texture>> mTextures;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_wavesRootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mOpaquePSO = nullptr;

		Resources::RenderItem* mWavesRitem = nullptr;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_tessInputLayout;
		
		//list of all render items.
		std::vector<std::unique_ptr<Resources::RenderItem>> mAllRitems;

		//Render items divided by PSO;
		std::vector<Resources::RenderItem*> mOpaqueRitems;

		Resources::PassConstants mMainPassCB;

		XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;
	};
}
