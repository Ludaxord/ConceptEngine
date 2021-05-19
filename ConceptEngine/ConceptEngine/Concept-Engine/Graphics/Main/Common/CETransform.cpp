#include "CETransform.h"

ConceptEngine::Graphics::Main::Common::CETransform::CETransform() : Matrix(),
                                                                    Translation(0.0f, 0.0f, 0.0f),
                                                                    Scale(1.0f, 1.0f, 1.0f),
                                                                    Rotation(0.0f, 0.0f, 0.0f) {
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetTranslation(float x, float y, float z) {
	SetTranslation(Math::CEVectorFloat3(x, y, z));
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetTranslation(const Math::CEVectorFloat3& position) {
	Translation = position;
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetScale(float x, float y, float z) {

}

void ConceptEngine::Graphics::Main::Common::CETransform::SetScale(const Math::CEVectorFloat3& scale) {
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetRotation(float x, float y, float z) {
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetRotation(const Math::CEVectorFloat3& rotation) {
}

void ConceptEngine::Graphics::Main::Common::CETransform::CalculateMatrix() {
	Math::CEVector translation = Math::CELoadFloat3(&Translation);
	Math::CEVector scale = Math::CELoadFloat3(&Scale);

	Math::CEVector rotation = Math::CEVectorSet(Rotation.z, Rotation.y, Rotation.z, 0.0f);

	Math::CEMatrix matrix = Math::CEMatrixMultiply(
		Math::CEMatrixMultiply(
			Math::CEMatrixScalingFromVector(scale),
			Math::CEMatrixRotationRollPitchYawFromVector(rotation)
		),
		Math::CEMatrixTranslationFromVector(translation)
	);
	Math::CEStoreFloat3x4(&TinyMatrix, matrix);
	matrix = Math::CEMatrixTranspose(matrix);
	Math::CEStoreFloat4x4(&Matrix, matrix);

	Math::CEMatrix matrixInverse = Math::CEMatrixInverse(nullptr, matrix);
	Math::CEStoreFloat4x4(&MatrixInverse, Math::CEMatrixTranspose(matrixInverse));
}
