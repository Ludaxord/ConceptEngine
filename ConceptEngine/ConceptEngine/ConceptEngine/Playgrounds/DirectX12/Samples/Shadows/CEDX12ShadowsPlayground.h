#pragma once

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Playground.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CECamera.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CECubeRenderTarget.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEShadowMap.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEWaves.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;
using namespace ConceptEngineFramework::Game;

namespace ConceptEngine::Playgrounds::DirectX12 {
	class CEDX12ShadowsPlayground final : public CEDX12Playground {
	public:
		CEDX12ShadowsPlayground();

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
		void BuildModelGeometry();
		void BuildFrameResources();
		void BuildMaterials();
		void BuildRenderItems();

		void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
		                     std::vector<Resources::RenderItem*>& ritems) const;

		void BuildCubeFaceCamera(float x, float y, float z);
		void BuildCubeDepthStencil();
		void AnimateSkullMovement(const CETimer& gt) const;
		void UpdateShadowTransform(const CETimer& gt);
		void UpdateShadowPassCB(const CETimer& gt);

		void DrawSceneToShadowMap();
		
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

		Resources::PassShadowConstants mMainPassCB;  // index 0 of pass cbuffer.
		Resources::PassShadowConstants mShadowPassCB;// index 1 of pass cbuffer.

		Resources::CECamera m_camera;

		Resources::CECamera mCubeMapCamera[6];
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_cubeDSV;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_cubeDepthStencilBuffer;

		CD3DX12_GPU_DESCRIPTOR_HANDLE m_nullSrv;

		UINT m_skyTexHeapIndex = 0;
		UINT m_shadowMapHeapIndex = 0;

		UINT m_nullCubeSrvIndex = 0;
		UINT m_nullTexSrvIndex = 0;

		Resources::RenderItem* mSkullRitem = nullptr;

		std::unique_ptr<Resources::CEShadowMap> m_shadowMap;
		DirectX::BoundingSphere mSceneBounds;

		float mLightNearZ = 0.0f;
		float mLightFarZ = 0.0f;
		XMFLOAT3 mLightPosW;
		XMFLOAT4X4 mLightView = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mLightProj = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mShadowTransform = Resources::MatrixIdentity4X4();

		float mLightRotationAngle = 0.0f;
		XMFLOAT3 mBaseLightDirections[3] = {
			XMFLOAT3(0.57735f, -0.57735f, 0.57735f),
			XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
			XMFLOAT3(0.0f, -0.707f, -0.707f)
		};
		XMFLOAT3 mRotatedLightDirections[3];
	};
}
