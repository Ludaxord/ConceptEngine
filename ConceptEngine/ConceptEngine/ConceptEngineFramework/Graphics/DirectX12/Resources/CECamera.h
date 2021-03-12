#pragma once

#ifndef CAMERA_H
#define CAMERA_H
#include <DirectXMath.h>

#include "CETypes.h"

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CECamera {
	public:
		CECamera();
		~CECamera();

		//Get/Set World camera position
		DirectX::XMVECTOR GetPosition() const;
		DirectX::XMFLOAT3 GetPosition3f() const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& v);

		//Get camera basis vector
		DirectX::XMVECTOR GetRight() const;
		DirectX::XMFLOAT3 GetRight3f() const;
		DirectX::XMVECTOR GetUp() const;
		DirectX::XMFLOAT3 GetUp3f() const;
		DirectX::XMVECTOR GetLook() const;
		DirectX::XMFLOAT3 GetLook3f() const;

		//Get frustum properties
		float GetNearZ() const;
		float GetFarZ() const;
		float GetAspect() const;
		float GetFovY() const;
		float GetFovX() const;

		//Get near and far plane dimensions in view space coordinates
		float GetNearWindowWidth() const;
		float GetNearWindowHeight() const;
		float GetFarWindowWidth() const;
		float GetFarWindowHeight() const;

		//set frustum
		void SetLens(float fovY, float aspect, float zn, float zf);

		//Define camera space via LookAt parameters
		void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
		void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

		//Get View/Proj matrices
		DirectX::XMMATRIX GetView() const;
		DirectX::XMMATRIX GetProj() const;

		DirectX::XMFLOAT4X4 GetView4x4f() const;
		DirectX::XMFLOAT4X4 GetProj4x4f() const;

		//Strafe/Walk camera a distance d.
		void Strafe(float d);
		void Walk(float d);

		//Rotate camera
		void Pitch(float angle);
		void RotateY(float angle);

		//After modifying camera position/orientation, call to rebuild view matrix
		void UpdateViewMatrix();
	protected:
	private:
		// Camera coordinate system with coordinates relative to world space.
		DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

		// Cache frustum properties.
		float mNearZ = 0.0f;
		float mFarZ = 0.0f;
		float mAspect = 0.0f;
		float mFovY = 0.0f;
		float mNearWindowHeight = 0.0f;
		float mFarWindowHeight = 0.0f;

		bool mViewDirty = true;

		//cache view/proj matrices.
		DirectX::XMFLOAT4X4 mView = Resources::MatrixIdentity4X4();
		DirectX::XMFLOAT4X4 mProj = Resources::MatrixIdentity4X4();
	};
}

#endif
