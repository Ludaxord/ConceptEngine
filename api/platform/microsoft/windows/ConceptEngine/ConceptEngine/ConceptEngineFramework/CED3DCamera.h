#pragma once
#include <DirectXMath.h>

#include "CECamera.h"

namespace Concept::GraphicsEngine::Direct3D::Camera {
	class CED3DCamera : public CECamera {
	public:
		CED3DCamera();
		~CED3DCamera() override;

		void XM_CALLCONV set_LookAt(DirectX::FXMVECTOR eye, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
		DirectX::XMMATRIX get_ViewMatrix() const;
		DirectX::XMMATRIX get_InverseViewMatrix() const;

		DirectX::XMMATRIX get_ProjectionMatrix() const;
		DirectX::XMMATRIX get_InverseProjectionMatrix() const;

		/*
		 * Set camera position in world-space
		 */
		void XM_CALLCONV set_Translation(DirectX::FXMVECTOR translation);
		DirectX::XMVECTOR get_Translation() const;

		/*
		 * Set camera rotation in world space
		 * @param rotation, rotation quaternion
		 */
		DirectX::XMVECTOR get_Rotation() const;

		void XM_CALLCONV Translate(DirectX::FXMVECTOR translation, Space space = Space::Local);
		void Rotate(DirectX::FXMVECTOR quaternion);

	protected:
		void UpdateViewMatrix() const override;
		void UpdateInverseViewMatrix() const override;
		void UpdateProjectionMatrix() const override;
		void UpdateInverseProjectionMatrix() const override;

		/*
		 * Data must be aligned otherwise, SSE intrinsics fail
		 * and throw exceptions.
		 */
		__declspec(align(16)) struct AlignedData {
			/*
			 * World space position of camera
			 */
			DirectX::XMVECTOR m_translation;
			/*
			 * World-space rotation of camera
			 * QUATERNION!!!
			 */
			DirectX::XMVECTOR m_rotation;

			DirectX::XMMATRIX m_viewMatrix, m_inverseViewMatrix;
			DirectX::XMMATRIX m_projectionMatrix, m_inverseProjectionMatrix;
		};

		AlignedData* pData;

	private:
	};
}
