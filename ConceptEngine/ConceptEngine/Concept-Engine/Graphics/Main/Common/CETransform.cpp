#include "CETransform.h"

ConceptEngine::Graphics::Main::Common::CETransform::CETransform() : Matrix(),
                                                                    Translation(0.0f, 0.0f, 0.0f),
                                                                    Scale(1.0f, 1.0f, 1.0f),
                                                                    Rotation(0.0f, 0.0f, 0.0f) {
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetTranslation(float x, float y, float z) {
	SetTranslation(CEVectorFloat3(x, y, z));
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetTranslation(const CEVectorFloat3& position) {
	Translation = position;
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetScale(float x, float y, float z) {
	SetScale(CEVectorFloat3(x, y, z));
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetScale(const CEVectorFloat3& scale) {
	Scale = scale;
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetRotation(float x, float y, float z) {
	SetRotation(CEVectorFloat3(x, y, z));
}

void ConceptEngine::Graphics::Main::Common::CETransform::SetRotation(const CEVectorFloat3& rotation) {
	Rotation = rotation;
	CalculateMatrix();
}

void ConceptEngine::Graphics::Main::Common::CETransform::CalculateMatrix() {
	CEVector translation = CELoadFloat3(&Translation);
	CEVector scale = CELoadFloat3(&Scale);

	CEVector rotation = CEVectorSet(Rotation.z, Rotation.y, Rotation.z, 0.0f);

	CEMatrix matrix = CEMatrixMultiply(
		CEMatrixMultiply(
			CEMatrixScalingFromVector(scale),
			CEMatrixRotationRollPitchYawFromVector(rotation)
		),
		CEMatrixTranslationFromVector(translation)
	);
	CEStoreFloat3x4(&TinyMatrix, matrix);
	matrix = CEMatrixTranspose(matrix);
	CEStoreFloat4x4(&Matrix, matrix);

	CEMatrix matrixInverse = CEMatrixInverse(nullptr, matrix);
	CEStoreFloat4x4(&MatrixInverse, CEMatrixTranspose(matrixInverse));
}
