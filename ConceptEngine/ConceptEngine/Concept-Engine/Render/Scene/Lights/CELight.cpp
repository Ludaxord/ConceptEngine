#include "CELight.h"

//TODO:Implement
ConceptEngine::Render::Scene::Lights::CELight::CELight(): Color(), ShadowBias(0.005f), MaxShadowBias(0.05f),
                                                          ShadowNearPlane(1.0f), ShadowFarPlane(30.0f) {
	CORE_OBJECT_INIT();
}

void ConceptEngine::Render::Scene::Lights::CELight::SetColor(const CEVectorFloat3& color) {
	Color = color;
}

void ConceptEngine::Render::Scene::Lights::CELight::SetColor(float R, float G, float B) {
	Color = CEVectorFloat3(R, G, B);
}

void ConceptEngine::Render::Scene::Lights::CELight::SetIntensity(float intensity) {
	Intensity = intensity;
}
