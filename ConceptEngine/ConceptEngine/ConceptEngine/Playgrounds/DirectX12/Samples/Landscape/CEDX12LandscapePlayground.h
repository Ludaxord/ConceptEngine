#pragma once
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEWaves.h"
using namespace ConceptEngineFramework::Graphics::DirectX12;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12LandscapePlayground final : public CEDX12Playground {
	public:
		CEDX12LandscapePlayground();

		void Create() override;
		void Update(const CETimer& gt) override;
		void Render(const CETimer& gt) override;
		void Resize() override;
		
		void OnMouseDown(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseUp(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnMouseMove(ConceptEngineFramework::Game::KeyCode key, int x, int y) override;
		void OnKeyUp(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnKeyDown(ConceptEngineFramework::Game::KeyCode key, char keyChar) override;
		void OnMouseWheel(ConceptEngineFramework::Game::KeyCode key, float wheelDelta, int x, int y) override;

		void UpdateCamera(const CETimer& gt) override;
		void UpdateObjectCBs(const CETimer& gt) override;
		void UpdateMainPassCB(const CETimer& gt) override;
	
	private:
		void UpdateWaves(const CETimer& gt);

		void BuildShadersAndInputLayout();
		void BuildLandscapeGeometry();
		void BuildGeometryBuffers();
		void BuildPSOs();
		void BuildFrameResources();
		void BuildRenderItems();
		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
		                     std::vector<Resources::RenderItem*>& ritems) const;

		float GetHillsHeight(float x, float z) const;
		XMFLOAT3 GetHillsNormal(float x, float z) const;

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		std::unordered_map<std::string, std::unique_ptr<Resources::MeshGeometry>> m_geometries;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shadersMap;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

		std::vector<std::unique_ptr<Resources::CEFrameResource>> mFrameResources;
		Resources::CEFrameResource* mCurrFrameResource = nullptr;
		int mCurrFrameResourceIndex = 0;

		UINT mCbvSrvDescriptorSize = 0;

		Resources::RenderItem* mWavesRitem = nullptr;

		// List of all the render items.
		std::vector<std::unique_ptr<Resources::RenderItem>> mAllRitems;

		// Render items divided by PSO.
		std::vector<Resources::RenderItem*> mRitemLayer[(int)Resources::RenderLayer::Count];

		std::unique_ptr<Resources::CEWaves> m_waves;

		Resources::PassConstants mMainPassCB;

		bool mIsWireframe = false;

		XMFLOAT3 mEyePos = {0.0f, 0.0f, 0.0f};
		float mSunTheta = 1.25f * XM_PI;
		float mSunPhi = XM_PIDIV4;
	};
}
