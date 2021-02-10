#pragma once
#include <d3d12.h>
#include <DirectXMath.h>


#include "CED3DCamera.h"
#include "CEPlayground.h"

namespace Concept {
	namespace GraphicsEngine::Direct3D {

		class CERenderTarget;
		class CECommandList;
		class CEPipelineStateObject;
		class CERootSignature;
		class CETexture;
		class CEScene;
		class CESwapChain;
		class CEGUI;
		class CEDevice;
	}

	namespace GameEngine::Playground {
		class CERayTracingPlayground : public CEPlayground {
		public:
			CERayTracingPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);

			bool LoadContent() override;
			void UnloadContent() override;
		protected:
			void OnUpdate(UpdateEventArgs& e) override;
			void OnRender() override;
			void OnKeyPressed(KeyEventArgs& e) override;
			void OnKeyReleased(KeyEventArgs& e) override;
			void OnMouseMoved(MouseMotionEventArgs& e) override;
			void OnMouseWheel(MouseWheelEventArgs& e) override;
			void OnResize(ResizeEventArgs& e) override;
			void OnDPIScaleChanged(DPIScaleEventArgs& e) override;

			void OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
				const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) const;

		private:
			/*
			 * DX12 Device
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEDevice> m_device;
			std::shared_ptr<GraphicsEngine::Direct3D::CESwapChain> m_swapChain;
			std::shared_ptr<GraphicsEngine::Direct3D::CEGUI> m_gui;

			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_cube;

			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_RTXRootSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_hitGroupRootSignature;

			D3D12_VIEWPORT m_viewPort;
			D3D12_RECT m_scissorRect;

			GraphicsEngine::Direct3D::Camera::CED3DCamera m_camera;

			struct alignas(16) CameraData {
				DirectX::XMVECTOR m_initialCamPos;
				DirectX::XMVECTOR m_initialCamRot;
				float m_InitialFov;
			};

			CameraData* m_pAlignedCameraData;

			float m_RenderScale;
		};
	}
}
