#pragma once
#include "../../Math/CEOperators.h"

namespace ConceptEngine::Render::Scene {
	struct CEAABB {
		CEVectorFloat3 GetCenter() const {
			return CEVectorFloat3();
		}

		float GetWidth() const {
			return Top.x - Bottom.x;
		}

		CEVectorFloat3 Top;
		CEVectorFloat3 Bottom;
	};
}
