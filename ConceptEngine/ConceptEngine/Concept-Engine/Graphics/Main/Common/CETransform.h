#pragma once
#include "../../../Math/CEOperators.h"

namespace ConceptEngine::Graphics::Main::Common {
	class CETransform {
	public:
		CETransform();
		~CETransform() = default;

		virtual void SetTranslation(float x, float y, float z);
		virtual void SetTranslation(const CEVectorFloat3& position);

		virtual void SetScale(float x, float y, float z);
		virtual void SetScale(const CEVectorFloat3& scale);

		void SetUniformScale(float scale) {
			SetScale(scale, scale, scale);
		}

		virtual void SetRotation(float x, float y, float z);
		virtual void SetRotation(const CEVectorFloat3& rotation);

		const CEVectorFloat3& GetTranslation() const {
			return Translation;
		}

		const CEVectorFloat3& GetScale() const {
			return Scale;
		}

		const CEVectorFloat3& GetRotation() const {
			return Rotation;
		}

		const CEMatrixFloat4X4& GetMatrix() const {
			return Matrix;
		}

		const CEMatrixFloat4X4& GetMatrixInverse() const {
			return MatrixInverse;
		}

		const CEMatrixFloat3X4& GetTinyMatrix() const {
			return TinyMatrix;
		}

	protected:
	private:
		void CalculateMatrix();

		CEMatrixFloat4X4 Matrix;
		CEMatrixFloat4X4 MatrixInverse;
		CEMatrixFloat3X4 TinyMatrix;

		CEVectorFloat3 Translation;
		CEVectorFloat3 Scale;
		CEVectorFloat3 Rotation;
	};
}
