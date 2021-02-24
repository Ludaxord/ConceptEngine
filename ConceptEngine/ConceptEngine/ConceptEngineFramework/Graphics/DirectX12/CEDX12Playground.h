#pragma once
#include <DirectXMath.h>
#include <memory>

#include "../CEPlayground.h"
#include "Resources/CETypes.h"

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
		virtual void OnMouseDown(Game::KeyCode key, int x, int y) override;
		virtual void OnMouseUp(Game::KeyCode key, int x, int y) override;
		virtual void OnMouseMove(Game::KeyCode key, int x, int y) override;
		virtual void OnKeyUp(Game::KeyCode key, std::wstring keyChar) override;
		virtual void OnKeyDown(Game::KeyCode key, std::wstring keyChar) override;
	protected:
		CEDX12Manager* m_dx12manager = nullptr;

		XMFLOAT4X4 mWorld = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mView = Resources::MatrixIdentity4X4();
		XMFLOAT4X4 mProj = Resources::MatrixIdentity4X4();

		float mTheta = 1.5f * XM_PI;
		float mPhi = XM_PIDIV4;
		float mRadius = 5.0f;

		POINT mLastMousePos;
	};
}
