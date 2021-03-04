#include "CEDX12BlendingWavesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12BlendingWavesPlayground::CEDX12BlendingWavesPlayground() : CEDX12Playground() {
}

void CEDX12BlendingWavesPlayground::Create() {
	CEDX12Playground::Create();
}

void CEDX12BlendingWavesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
}

void CEDX12BlendingWavesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
}

void CEDX12BlendingWavesPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CEDX12BlendingWavesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12BlendingWavesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12BlendingWavesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	static const float Pi = 3.1415926535f;
	if (key == KeyCode::LButton) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = m_dx12manager->Clamp(mPhi, 0.1f, Pi - 0.1f);
	}
	else if (key == KeyCode::RButton) {
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = m_dx12manager->Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CEDX12BlendingWavesPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12BlendingWavesPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::Left:
		mSunTheta -= 4.0f * dt;
		break;
	case KeyCode::Right:
		mSunTheta += 4.0f * dt;
		break;
	case KeyCode::Up:
		mSunPhi -= 4.0f * dt;
		break;
	case KeyCode::Down:
		mSunPhi += 4.0f * dt;
		break;
	}

	mSunPhi = m_dx12manager->Clamp(mSunPhi, 0.1f, XM_PIDIV2);
}

void CEDX12BlendingWavesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12BlendingWavesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void CEDX12BlendingWavesPlayground::AnimateMaterials(const CETimer& gt) {
	CEDX12Playground::AnimateMaterials(gt);
}

void CEDX12BlendingWavesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
}

void CEDX12BlendingWavesPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
}

void CEDX12BlendingWavesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
}

void CEDX12BlendingWavesPlayground::LoadTextures() {
}

void CEDX12BlendingWavesPlayground::BuildDescriptorHeaps() {
}

void CEDX12BlendingWavesPlayground::BuildLandGeometry() {
}

void CEDX12BlendingWavesPlayground::BuildWavesGeometryBuffers() {
}

void CEDX12BlendingWavesPlayground::BuildBoxGeometry() {
}

void CEDX12BlendingWavesPlayground::BuildMaterials() {
}

void CEDX12BlendingWavesPlayground::BuildRenderItems() {
}

void CEDX12BlendingWavesPlayground::BuildFrameResources() {
}

void CEDX12BlendingWavesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
	std::vector<Resources::RenderItem*>& ritems) const {
}

float CEDX12BlendingWavesPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12BlendingWavesPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void CEDX12BlendingWavesPlayground::UpdateWaves(const CETimer& gt) {
}
