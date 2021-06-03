#pragma once

#include "../../../Graphics/Main/Objects/CEObject.h"
#include "../../../Math/CEOperators.h"

namespace ConceptEngine::Render::Scene::Lights {
	class CELight : public Graphics::Main::Objects::CEObject {
	CORE_OBJECT(CELight, CEObject);

	public:
		CELight();
		virtual ~CELight() = default;

		void SetColor(const CEVectorFloat3& color);
		void SetColor(float R, float G, float B);

		void SetIntensity(float intensity);

		void SetShadowBias(float shadowBias) {
			ShadowBias = shadowBias;
		}

		void SetMaxShadowBias(float maxShadowBias) {
			MaxShadowBias = maxShadowBias;
		}

		float GetIntensity() const {
			return Intensity;
		}

		const CEVectorFloat3& GetColor() const {
			return Color;
		}

		float GetShadowNearPlane() const {
			return ShadowNearPlane;
		}

		float GetShadowFarPlane() const {
			return ShadowFarPlane;
		}

		float GetShadowBias() const {
			return ShadowBias;
		}

		float GetMaxShadowBias() const {
			return MaxShadowBias;
		}

	protected:
		CEVectorFloat3 Color;
		float Intensity = 1.0f;
		float ShadowNearPlane;
		float ShadowFarPlane;
		float ShadowBias;
		float MaxShadowBias;
	};
}
