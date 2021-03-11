#include "CECamera.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CECamera::CECamera() {
	SetLens(0.25f * Resources::Pi, 1.0f, 1.0f, 1000.0f);
}

CECamera::~CECamera() {
}

DirectX::XMVECTOR CECamera::GetPosition() const {
	return XMLoadFloat3(&m_position);
}

DirectX::XMFLOAT3 CECamera::GetPosition3f() const {
	return m_position;
}

void CECamera::SetPosition(float x, float y, float z) {
	m_position = XMFLOAT3(x, y, z);
	m_viewDirty = true;
}

void CECamera::SetPosition(const DirectX::XMFLOAT3& v) {
	m_position = v;
	m_viewDirty = true;
}

DirectX::XMVECTOR CECamera::GetRight() const {
	return XMLoadFloat3(&m_right);
}

DirectX::XMFLOAT3 CECamera::GetRight3f() const {
	return m_right;
}

DirectX::XMVECTOR CECamera::GetUp() const {
	return XMLoadFloat3(&m_up);
}

DirectX::XMFLOAT3 CECamera::GetUp3f() const {
	return m_up;
}

DirectX::XMVECTOR CECamera::GetLook() const {
	return XMLoadFloat3(&m_look);
}

DirectX::XMFLOAT3 CECamera::GetLook3f() const {
	return m_look;
}

float CECamera::GetNearZ() const {
	return m_nearZ;
}

float CECamera::GetFarZ() const {
	return m_farZ;
}

float CECamera::GetAspect() const {
	return m_aspect;
}

float CECamera::GetFovY() const {
	return m_fovY;
}

float CECamera::GetFovX() const {
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / m_nearZ);
}

float CECamera::GetNearWindowWidth() const {
	return m_aspect * m_nearWindowHeight;
}

float CECamera::GetNearWindowHeight() const {
	return m_nearWindowHeight;
}

float CECamera::GetFarWindowWidth() const {
	return m_aspect * m_farWindowHeight;
}

float CECamera::GetFarWindowHeight() const {
	return m_farWindowHeight;
}

void CECamera::SetLens(float fovY, float aspect, float zn, float zf) {
	//cache properties
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = zn;
	m_farZ = zf;
}

void CECamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp) {
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_look, L);
	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);

	m_viewDirty = true;
}

void CECamera::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up) {
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);

	m_viewDirty = true;
}

DirectX::XMMATRIX CECamera::GetView() const {
	assert(!m_viewDirty);
	return XMLoadFloat4x4(&m_view);
}

DirectX::XMMATRIX CECamera::GetProj() const {
	return XMLoadFloat4x4(&m_proj);
}

DirectX::XMFLOAT4X4 CECamera::GetView4x4f() const {
	assert(!m_viewDirty);
	return m_view;
}

DirectX::XMFLOAT4X4 CECamera::GetProj4x4f() const {
	return m_proj;
}

void CECamera::Strafe(float d) {
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));
	m_viewDirty = true;
}

void CECamera::Walk(float d) {
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));

	m_viewDirty = true;
}

void CECamera::Pitch(float angle) {
	//Rotate up and look vector about right vector
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));;
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));;

	m_viewDirty = true;
}

void CECamera::RotateY(float angle) {
	//Rotate basis vectors about world y-axis

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

	m_viewDirty = true;
}

void CECamera::UpdateViewMatrix() {
	if (m_viewDirty) {

		XMVECTOR R = XMLoadFloat3(&m_right);
		XMVECTOR U = XMLoadFloat3(&m_up);
		XMVECTOR L = XMLoadFloat3(&m_look);
		XMVECTOR P = XMLoadFloat3(&m_position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&m_right, R);
		XMStoreFloat3(&m_up, U);
		XMStoreFloat3(&m_look, L);

		m_view(0, 0) = m_right.x;
		m_view(1, 0) = m_right.y;
		m_view(2, 0) = m_right.z;
		m_view(3, 0) = x;

		m_view(0, 1) = m_up.x;
		m_view(1, 1) = m_up.y;
		m_view(2, 1) = m_up.z;
		m_view(3, 1) = y;

		m_view(0, 2) = m_look.x;
		m_view(1, 2) = m_look.y;
		m_view(2, 2) = m_look.z;
		m_view(3, 2) = z;

		m_view(0, 3) = 0.0f;
		m_view(1, 3) = 0.0f;
		m_view(2, 3) = 0.0f;
		m_view(3, 3) = 1.0f;

		m_viewDirty = false;
	}
}
