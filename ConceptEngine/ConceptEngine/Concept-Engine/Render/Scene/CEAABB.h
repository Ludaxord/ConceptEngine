#pragma once
#include "../../Math/CEOperators.h"

namespace ConceptEngine::Render::Scene {
	struct CEAABB {
		CEVectorFloat3 GetCenter() const {
			return CEVectorFloat3((Bottom.x + Top.x) * 0.5f, (Bottom.y + Top.y) * 0.5f, (Bottom.z + Bottom.z) * 0.5f);
		}

		float GetWidth() const {
			return Top.x - Bottom.x;
		}

		float GetHeight() const {
			return Top.y - Bottom.y;
		}

		float GetDepth() const {
			return Top.z - Bottom.z;
		}

		CEVectorFloat3 Top;
		CEVectorFloat3 Bottom;
	};
}
