#pragma once
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEUploadBuffer.h"
using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12CratePlayground final : public CEDX12Playground {

	public:
		CEDX12CratePlayground();
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
		void BuildDescriptorHeaps();
		void LoadTextures();
		void BuildBoxGeometry();
		void BuildMaterials();
		void BuildRenderItems();
		void BuildFrameResources();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
			std::vector<Resources::RenderItem*>& ritems) const;
	private:

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;

		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		std::vector<Resources::RenderItem*> mOpaqueRitems;
		Resources::PassConstants mMainPassCB;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> mGeometries;
		std::unordered_map<std::string, std::unique_ptr<Resources::Material>> mMaterials;
		std::unordered_map<std::string, std::shared_ptr<Resources::Texture>> mTextures;

		XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;
	};
}
