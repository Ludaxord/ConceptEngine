#pragma once
#include "CELight.h"

namespace ConceptEngine::Render::Scene::Lights {
	class CESpotLight : public CELight {
	CORE_OBJECT(CESpotLight, CELight);

	public:
		CESpotLight();
		~CESpotLight();

		void SetConeAngle(float coneAngle);

		inline float GetConeAngle() const {
			return ConeAngle;
		}

	private:
		float ConeAngle;
	};
}
