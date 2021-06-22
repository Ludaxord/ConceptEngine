#include "CEDirectionalLight.h"

ConceptEngine::Render::Scene::Lights::CEDirectionalLight::CEDirectionalLight(): CELight(), Direction(0.0f, -1.0f, 0.0f),
	Rotation(0.0f, 0.0f, 0.0f), ShadowMapPosition(0.0f, 0.0f, 0.0f), LookAt(0.0f, 0.0f, 0.0f), Matrix() {
	CORE_OBJECT_INIT();

	CEStoreFloat4x4(&Matrix, CEMatrixIdentity());
}

ConceptEngine::Render::Scene::Lights::CEDirectionalLight::~CEDirectionalLight() {
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetRotation(const CEVectorFloat3& rotation) {
	Rotation = rotation;
	CalculateMatrix();
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetRotation(float x, float y, float z) {
	SetRotation(CEVectorFloat3(x, y, z));
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetLookAt(const CEVectorFloat3& lookAt) {
	LookAt = lookAt;
	CalculateMatrix();
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetLookAt(float x, float y, float z) {
	SetLookAt(CEVectorFloat3(x, y, z));
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetShadowNearPlane(float shadowNearPlane) {
	if (shadowNearPlane > 0.0f) {
		if (abs(ShadowFarPlane - shadowNearPlane) >= 0.1f) {
			ShadowNearPlane = shadowNearPlane;
			CalculateMatrix();
		}
	}
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::SetShadowFarPlane(float shadowFarPlane) {
	if (shadowFarPlane > 0.0f) {
		if (abs(shadowFarPlane - ShadowNearPlane) >= 0.1) {
			ShadowFarPlane = shadowFarPlane;
			CalculateMatrix();
		}
	}
}

void ConceptEngine::Render::Scene::Lights::CEDirectionalLight::CalculateMatrix() {
	auto direction = CEVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	auto rotation = CEMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	auto offset=  CEVector4Transform(direction, rotation);
	direction = CEVector3Normalize(offset);
	    CEStoreFloat3(&Direction, direction);

    const float Scale = (ShadowFarPlane - ShadowNearPlane) / 2.0f;
    offset = CEVectorScale(offset, -Scale);

    auto lookAt   = CELoadFloat3(&LookAt);
    auto position = CEVectorAdd(lookAt, offset);
    CEStoreFloat3(&ShadowMapPosition, position);

    auto XmUp = CEVectorSet(0.0, 0.0f, 1.0f, 0.0f);
    XmUp = CEVector3Normalize(CEVector3Transform(XmUp, rotation));

    const float Offset    = 35.0f;
    auto projection = CEMatrixOrthographicOffCenterLH(-Offset, Offset, -Offset, Offset, ShadowNearPlane, ShadowFarPlane);
    auto view       = CEMatrixLookAtLH(position, lookAt, XmUp);

    CEStoreFloat4x4(&Matrix, CEMatrixMultiplyTranspose(view, projection));
}
