#pragma once
#include "CELight.h"

namespace ConceptEngine::Render::Scene::Lights {
	class CEDirectionalLight : public CELight {
	CORE_OBJECT(CEDirectionalLight, CELight);

	public:
		CEDirectionalLight();
		~CEDirectionalLight();

		void SetRotation(const CEVectorFloat3& rotation);
		void SetRotation(float x, float y, float z);

		void SetLookAt(const CEVectorFloat3& lookAt);
		void SetLookAt(float x, float y, float z);

		void SetShadowNearPlane(float shadowNearPlane);
		void SetShadowFarPlane(float shadowFarPlane);

		const CEVectorFloat3& GetDirection() const {
			return Direction;
		}

		const CEVectorFloat3& GetRotation() const {
			return Rotation;
		}

		const CEVectorFloat3& GetShadowMapPosition() const {
			return ShadowMapPosition;
		}

		const CEVectorFloat3& GetLookAt() const {
			return LookAt;
		}

		const CEMatrixFloat4X4& GetMatrix() const {
			return Matrix;
		}

	private:
		void CalculateMatrix();

		CEVectorFloat3 Direction;
		CEVectorFloat3 Rotation;
		CEVectorFloat3 LookAt;
		CEVectorFloat3 ShadowMapPosition;
		CEMatrixFloat4X4 Matrix;
	};
}
