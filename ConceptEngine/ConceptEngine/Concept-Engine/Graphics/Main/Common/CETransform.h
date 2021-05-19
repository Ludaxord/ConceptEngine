#pragma once
#include "../../../Math/CEMathTypes.h"

namespace ConceptEngine::Graphics::Main::Common {
	class CETransform {
	public:
		CETransform();
		~CETransform() = default;

		virtual void SetTranslation(float x, float y, float z);
		virtual void SetTranslation(const Math::CEVectorFloat3& position);

		virtual void SetScale(float x, float y, float z);
		virtual void SetScale(const Math::CEVectorFloat3& scale);

		void SetUniformScale(float scale) {
			SetScale(scale, scale, scale);
		}

		virtual void SetRotation(float x, float y, float z);
		virtual void SetRotation(const Math::CEVectorFloat3& rotation);

		const Math::CEVectorFloat3& GetTranslation() const {
			return Translation;
		}

		const Math::CEVectorFloat3& GetScale() const {
			return Scale;
		}

		const Math::CEVectorFloat3& GetRotation() const {
			return Rotation;
		}

		const Math::CEMatrixFloat4X4& GetMatrix() const {
			return Matrix;
		}

		const Math::CEMatrixFloat4X4& GetMatrixInverse() const {
			return MatrixInverse;
		}

		const Math::CEMatrixFloat3X4& GetTinyMatrix() const {
			return TinyMatrix;
		}

	protected:
	private:
		void CalculateMatrix();

		Math::CEMatrixFloat4X4 Matrix;
		Math::CEMatrixFloat4X4 MatrixInverse;
		Math::CEMatrixFloat3X4 TinyMatrix;

		Math::CEVectorFloat3 Translation;
		Math::CEVectorFloat3 Scale;
		Math::CEVectorFloat3 Rotation;
	};
}
