#pragma once

#include "../../Math/CEOperators.h"

namespace ConceptEngine::Render::Scene {
	class CECamera {
	public:
		CECamera();
		~CECamera() = default;

		void Move(float X, float Y, float Z);

		void Rotate(float pitch, float yaw, float roll);

		void UpdateMatrices();

		const CEMatrixFloat4X4& GetViewMatrix() const {
			return View;
		}

		const CEMatrixFloat4X4& GetViewInverseMatrix() const {
			return ViewInverse;
		}

		const CEMatrixFloat4X4& GetProjectionMatrix() const {
			return Projection;
		}

		const CEMatrixFloat4X4& GetProjectionInverseMatrix() const {
			return ProjectionInverse;
		}

		const CEMatrixFloat4X4& GetViewProjectionMatrix() const {
			return ViewProjection;
		}

		const CEMatrixFloat4X4& GetViewProjectionInverseMatrix() const {
			return ViewProjectionInverse;
		}

		const CEMatrixFloat4X4& GetViewProjectionWithoutTranslateMatrix() const {
			return ViewProjectionNoTranslation;
		}

		CEVectorFloat3 GetPosition() const {
			return Position;
		}

		CEVectorFloat3 GetForward() const {
			return Forward;
		}

		CEVectorFloat3 GetUp() const {
			return Up;
		}

		CEVectorFloat3 GetRight() const {
			return Right;
		}

		float GetNearPlane() const {
			return NearPlane;
		}

		float GetFarPlane() const {
			return FarPlane;
		}

		float GetAspectRatio() const {
			return AspectRatio;
		}

	private:
		CEMatrixFloat4X4 View;
		CEMatrixFloat4X4 ViewInverse;
		CEMatrixFloat4X4 Projection;
		CEMatrixFloat4X4 ProjectionInverse;
		CEMatrixFloat4X4 ViewProjection;
		CEMatrixFloat4X4 ViewProjectionInverse;
		CEMatrixFloat4X4 ViewProjectionNoTranslation;
		float NearPlane;
		float FarPlane;
		float AspectRatio;
		CEVectorFloat3 Position;
		CEVectorFloat3 Rotation;
		CEVectorFloat3 Forward;
		CEVectorFloat3 Right;
		CEVectorFloat3 Up;
	};
}
