#pragma once

struct CERigidTransform {
	CERigidTransform(const DirectX::XMFLOAT3& InTranslation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
	                 const DirectX::XMFLOAT4& InQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
	                 const DirectX::XMFLOAT3& InScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f)) :
		Translation(InTranslation),
		Quaternion(InQuaternion),
		Scale(InScale) {
	}

	DirectX::XMFLOAT3 GetForward() {
		DirectX::XMVECTOR Quat = XMLoadFloat4(&Quaternion);
		auto Mat = DirectX::XMMatrixRotationQuaternion(Quat);
		DirectX::XMFLOAT3 Forward;
		XMStoreFloat3(&Forward, Mat.r[2]);

		return Forward;
	}

	DirectX::XMFLOAT3 GetRight() {
		DirectX::XMVECTOR Quat = XMLoadFloat4(&Quaternion);
		auto Mat = DirectX::XMMatrixRotationQuaternion(Quat);
		DirectX::XMFLOAT3 Right;
		XMStoreFloat3(&Right, Mat.r[0]);

		return Right;
	}

	DirectX::XMFLOAT3 GetUp() {
		DirectX::XMVECTOR Quat = XMLoadFloat4(&Quaternion);
		auto Mat = DirectX::XMMatrixRotationQuaternion(Quat);
		DirectX::XMFLOAT3 Up;
		XMStoreFloat3(&Up, Mat.r[1]);

		return Up;
	}

	DirectX::XMFLOAT3 Translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 Quaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
};

inline XMMATRIX RigidTransformToMatrix(const CERigidTransform& Transform) {
	XMVECTOR Scale = XMLoadFloat3(&Transform.Scale);
	XMVECTOR Translation = XMLoadFloat3(&Transform.Translation);
	XMVECTOR Quaternion = XMLoadFloat4(&Transform.Quaternion);

	XMVECTOR ZeroVector = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	return XMMatrixAffineTransformation(Scale, ZeroVector, Quaternion, Translation);
}

inline CERigidTransform RotateRigidTransformLocal(const CERigidTransform Transform, XMFLOAT3 Axis, float Angle) {
	CERigidTransform ResultTransform = Transform;
	XMMATRIX Matrix = RigidTransformToMatrix(Transform);

	Matrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX Rotation = XMMatrixRotationAxis(XMLoadFloat3(&Axis), Angle);
	XMMATRIX ResultMatrix = Matrix * Rotation;

	XMVECTOR MeshWorldPositionVector;
	XMVECTOR MeshWorldQuaternionVector;
	XMVECTOR MeshWorldScaleVector;
	XMMatrixDecompose(&MeshWorldScaleVector, &MeshWorldQuaternionVector, &MeshWorldPositionVector, ResultMatrix);

	XMStoreFloat4(&ResultTransform.Quaternion, MeshWorldQuaternionVector);

	return ResultTransform;
}
