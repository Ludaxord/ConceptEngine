#pragma once
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEWaves.h"
using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12LandscapePlayground final : public CEDX12Playground {
	public:
		CEDX12LandscapePlayground();

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
		void UpdateWaves(const CETimer& gt);

		void BuildLandscapeGeometry();
		void BuildGeometryBuffers();
		void BuildFrameResources();
		void BuildRenderItems();
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
		                     std::vector<Resources::RenderItem*>& ritems) const;

		float GetHillsHeight(float x, float z) const;
		XMFLOAT3 GetHillsNormal(float x, float z) const;

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> m_geometries;

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;
		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		UINT mCbvSrvDescriptorSize = 0;

		Resources::RenderItem* mWavesRitem = nullptr;

		std::unique_ptr<Resources::CEWaves> m_waves;

		Resources::PassConstants mMainPassCB;

		XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;
	};
}
