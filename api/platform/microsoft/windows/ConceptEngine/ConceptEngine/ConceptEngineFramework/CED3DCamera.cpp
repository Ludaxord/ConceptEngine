#include "CED3DCamera.h"

using namespace Concept::GraphicsEngine::Direct3D::Camera;

using namespace DirectX;

CED3DCamera::CED3DCamera(): CECamera() {
	pData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
	pData->m_translation = DirectX::XMVectorZero();
	pData->m_rotation = DirectX::XMQuaternionIdentity();
}

CED3DCamera::~CED3DCamera() {
	_aligned_free(pData);
}

void XM_CALLCONV CED3DCamera::set_LookAt(FXMVECTOR eye, FXMVECTOR target, FXMVECTOR up) {
	pData->m_viewMatrix = XMMatrixLookAtLH(eye, target, up);

	pData->m_translation = eye;
	pData->m_rotation = XMQuaternionRotationMatrix(XMMatrixTranspose(pData->m_viewMatrix));
}

DirectX::XMMATRIX CED3DCamera::get_ViewMatrix() const {
	if (m_viewDirty) {
		UpdateViewMatrix();
	}
	return pData->m_viewMatrix;
}

DirectX::XMMATRIX CED3DCamera::get_InverseViewMatrix() const {
	if (m_inverseViewDirty) {
		pData->m_inverseViewMatrix = XMMatrixInverse(nullptr, pData->m_viewMatrix);
		m_inverseViewDirty = false;
	}
	return pData->m_inverseViewMatrix;
}

DirectX::XMMATRIX CED3DCamera::get_ProjectionMatrix() const {
	if (m_projectionDirty) {
		UpdateProjectionMatrix();
	}

	return pData->m_projectionMatrix;
}

DirectX::XMMATRIX CED3DCamera::get_InverseProjectionMatrix() const {
	if (m_inverseProjectionDirty) {
		UpdateInverseProjectionMatrix();
	}
	return pData->m_inverseProjectionMatrix;
}

void CED3DCamera::set_Translation(DirectX::FXMVECTOR translation) {
	pData->m_translation = translation;
	m_viewDirty = true;
}

DirectX::XMVECTOR CED3DCamera::get_Translation() const {
	return pData->m_translation;
}

DirectX::XMVECTOR CED3DCamera::get_Rotation() const {
	return pData->m_rotation;
}

void XM_CALLCONV CED3DCamera::Translate(FXMVECTOR translation, Space space) {
	switch (space) {
	case Space::Local: {
		pData->m_translation += XMVector3Rotate(translation, pData->m_rotation);
	}
	break;
	case Space::World: {
		pData->m_translation += translation;
	}
	break;
	}

	pData->m_translation = XMVectorSetW(pData->m_translation, 1.0f);
	m_viewDirty = true;
	m_inverseViewDirty = true;
}

void CED3DCamera::Rotate(DirectX::FXMVECTOR quaternion) {
	pData->m_rotation = XMQuaternionMultiply(quaternion, pData->m_rotation);

	m_viewDirty = true;
	m_inverseViewDirty = true;
}

void CED3DCamera::UpdateViewMatrix() const {
	XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(pData->m_rotation));
	XMMATRIX translationMatrix = XMMatrixTranslationFromVector(-(pData->m_translation));

	pData->m_viewMatrix = translationMatrix * rotationMatrix;

	m_inverseViewDirty = true;
	m_viewDirty = false;
}

void CED3DCamera::UpdateInverseViewMatrix() const {
	if (m_viewDirty) {
		UpdateViewMatrix();
	}

	pData->m_inverseViewMatrix = XMMatrixInverse(nullptr, pData->m_viewMatrix);
	m_inverseViewDirty = false;
}

void CED3DCamera::UpdateProjectionMatrix() const {
	pData->m_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_vFoV), m_aspectRatio, m_zNear, m_zFar);

	m_projectionDirty = false;
	m_inverseProjectionDirty = true;
}

void CED3DCamera::UpdateInverseProjectionMatrix() const {
	if (m_projectionDirty) {
		UpdateProjectionMatrix();
	}

	pData->m_inverseProjectionMatrix = XMMatrixInverse(nullptr, pData->m_projectionMatrix);
	m_inverseProjectionDirty = false;
}
