#pragma once
#include <DirectXMath.h>
#include <memory>

#include "../CEPlayground.h"
#include "Resources/CETypes.h"

inline const int gNumFrameResources = 3;

namespace ConceptEngineFramework::Graphics::DirectX12 {
	class CEDX12Manager;

	class CEDX12Playground : public CEPlayground {
	public:
		CEDX12Playground();

		virtual void Init(CEGraphicsManager* manager) override;
		virtual void Create() override;
		virtual void Update(const CETimer& gt) override;
		virtual void Render(const CETimer& gt) override;
		virtual void Resize() override;
		virtual void UpdateCamera(const CETimer& gt) override;
		virtual void AnimateMaterials(const CETimer& gt) override;

		virtual void OnMouseDown(Game::KeyCode key, int x, int y) override;
		virtual void OnMouseUp(Game::KeyCode key, int x, int y) override;

		//TODO: move world movement to another function to not copy and paste same code in loop
		virtual void OnMouseMove(Game::KeyCode key, int x, int y) override;
		virtual void OnKeyUp(Game::KeyCode key, char keyChar, const CETimer& gt) override;
		virtual void OnKeyDown(Game::KeyCode key, char keyChar, const CETimer& gt) override;
		virtual void OnMouseWheel(Game::KeyCode key, float wheelDelta, int x, int y) override;

		virtual void UpdateObjectCBs(const CETimer& gt);
		virtual void UpdateMainPassCB(const CETimer& gt);
		virtual void UpdateMaterialCBs(const CETimer& gt);
	protected:
		virtual void BuildColorShadersAndInputLayout(std::string vertexShaderFileName, std::string pixelShaderFileName);
		virtual void
		BuildNormalShadersAndInputLayout(std::string vertexShaderFileName, std::string pixelShaderFileName);
		virtual void BuildInputLayout();
		virtual void BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature);

		CEDX12Manager* m_dx12manager = nullptr;

		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shadersMap;
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;

		// List of all the render items.
		std::vector<std::unique_ptr<Resources::RenderItem>> mAllRitems;

		// Render items divided by PSO.
		std::vector<Resources::RenderItem*> mRitemLayer[(int)Resources::RenderLayer::Count];

		XMFLOAT4X4 mWorld = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mView = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mProj = Resources::MatrixIdentity4X4();

		bool mIsWireframe = false;

		float mTheta = 1.5f * XM_PI;
		float mPhi = XM_PIDIV4;
		float mRadius = 5.0f;

		float m_forward;
		float m_backward;
		float m_up;
		float m_down;
		float m_left;
		float m_right;

		bool m_shift;

		POINT mLastMousePos;
	};
}
