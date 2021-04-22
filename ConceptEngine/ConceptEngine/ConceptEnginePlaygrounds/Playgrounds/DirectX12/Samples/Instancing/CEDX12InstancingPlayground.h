#pragma once


#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CECamera.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEWaves.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12InstancingPlayground final : public CEDX12Playground {
	public:
		CEDX12InstancingPlayground();

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

		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;
		void UpdateMaterialCBs(const CETimer& gt) override;
	protected:
		void BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) override;

	private:
		void LoadTextures();
		void BuildDescriptorHeaps();
		void BuildShapesGeometry();
		void BuildFrameResources();
		void BuildMaterials();
		void BuildRenderItems();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
		                     std::vector<Resources::RenderItem*>& ritems) const;

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;

		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> mGeometries;
		std::unordered_map<std::string, std::unique_ptr<Resources::Material>> mMaterials;
		std::unordered_map<std::string, std::shared_ptr<Resources::Texture>> mTextures;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		//list of all render items.
		std::vector<std::unique_ptr<Resources::RenderItem>> mAllRitems;

		//Render items divided by PSO;
		std::vector<Resources::RenderItem*> mOpaqueRitems;

		Resources::PassStructuredConstants mMainPassCB;

		Resources::CECamera m_camera;

		UINT m_instanceCount = 0;
		UINT m_visibleInstanceCount = -1;

		bool mFrustumCullingEnabled = true;

		BoundingFrustum mCamFrustum;
	};
}
