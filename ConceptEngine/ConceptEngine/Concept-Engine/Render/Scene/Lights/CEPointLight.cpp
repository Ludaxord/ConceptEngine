#include "CEPointLight.h"
#include <cstdlib>

using namespace ConceptEngine::Render::Scene::Lights;

//TODO: Change all DirectX specific Matrix to CE Matrix class.
CEPointLight::CEPointLight(): CELight(), Position(0.0f, 0.0f, 0.0f), Matrices() {
	CORE_OBJECT_INIT();
	CalculateMatrices();
}

void CEPointLight::SetPosition(const CEVectorFloat3& position) {
	Position = position;
}

void CEPointLight::SetPosition(float x, float y, float z) {
	SetPosition(CEVectorFloat3(x, y, z));
}

void CEPointLight::SetShadowNearPlane(float shadowNearPlane) {
	if (shadowNearPlane > 0.0f) {
		if (abs(ShadowFarPlane - shadowNearPlane) >= 0.1f) {
			ShadowNearPlane = shadowNearPlane;
			CalculateMatrices();
		}
	}
}

void CEPointLight::SetShadowFarPlane(float shadowFarPlane) {
	if (shadowFarPlane > 0.0f) {
		if (abs(shadowFarPlane - ShadowNearPlane) >= 0.1f) {
			ShadowFarPlane = shadowFarPlane;
			CalculateMatrices();
		}
	}
}

void CEPointLight::CalculateMatrices() {
	if (!ShadowCaster) {
		return;
	}

	XMFLOAT3 Directions[6] = {
		{1.0f, 0.0f, 0.0f},
		{-1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, -1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, -1.0f},
	};

	XMFLOAT3 UpVectors[6] = {
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, -1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
	};

	XMVECTOR LightPosition = XMLoadFloat3(&Position.Native);
	for (uint32 i = 0; i < 6; i++) {
		XMVECTOR LightDirection = XMLoadFloat3(&Directions[i]);
		XMVECTOR LightUp = XMLoadFloat3(&UpVectors[i]);

		XMMATRIX LightProjection = XMMatrixPerspectiveFovLH(XM_PI / 2.0f, 1.0f, ShadowNearPlane, ShadowFarPlane);
		XMStoreFloat4x4(&ProjectionMatrices[i].Native, LightProjection);

		XMMATRIX LightView = XMMatrixLookToLH(LightPosition, LightDirection, LightUp);
		XMStoreFloat4x4(&ViewMatrices[i].Native, XMMatrixTranspose(LightView));
		XMStoreFloat4x4(&Matrices[i].Native, XMMatrixMultiplyTranspose(LightView, LightProjection));
	}
}
