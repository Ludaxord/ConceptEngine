#pragma once
#include "CELight.h"

namespace ConceptEngine::Render::Scene::Lights {
	class CEPointLight : public CELight {
	CORE_OBJECT(CEPointLight, CELight);

	public:
		CEPointLight();
		~CEPointLight() = default;

		void SetPosition(const CEVectorFloat3& position);
		void SetPosition(float x, float y, float z);

		void SetShadowNearPlane(float shadowNearPlane);
		void SetShadowFarPlane(float shadowFarPlane);

		void SetShadowCaster(bool shadowCaster) {
			ShadowCaster = shadowCaster;
			CalculateMatrices();
		}

		bool IsShadowCaster() const {
			return ShadowCaster;
		}

		const CEVectorFloat3& GetPosition() const {
			return Position;
		}

		const CEMatrixFloat4X4& GetMatrix(uint32 index) const {
			Assert(index < 6);
			return Matrices[index];
		}

		const CEMatrixFloat4X4& GetViewMatrix(uint32 index) const {
			Assert(index < 6);
			return ViewMatrices[index];
		}

		const CEMatrixFloat4X4& GetProjectionMatrix(uint32 index) const {
			Assert(index < 6);
			return ProjectionMatrices[index];
		}

	private:
		void CalculateMatrices();

		CEMatrixFloat4X4 Matrices[6];
		CEMatrixFloat4X4 ViewMatrices[6];
		CEMatrixFloat4X4 ProjectionMatrices[6];
		CEVectorFloat3 Position;
		bool ShadowCaster = false;
	};
}
