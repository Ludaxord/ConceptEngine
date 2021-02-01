#pragma once
#include "CED3DCamera.h"
#include "CED3DLight.h"
#include "CEPlayground.h"
#include "CERenderTarget.h"

namespace Concept {
	namespace GraphicsEngine::Direct3D {
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
		class CEBasicLightingPlayground : public CEPlayground {
		public:
			CEBasicLightingPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
			virtual ~CEBasicLightingPlayground();

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

			void OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList, const GraphicsEngine::Direct3D::CERenderTarget& renderTarget);

		private:
			/*
			 * DX12 Device
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEDevice> m_device;
			std::shared_ptr<GraphicsEngine::Direct3D::CESwapChain> m_swapChain;
			std::shared_ptr<GraphicsEngine::Direct3D::CEGUI> m_gui;

			/*
			 * Some geometry to render
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_cube;
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_sphere;
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_cone;
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_torus;
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_plane;

			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_defaultTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_directXTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_earthTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_monaLisaTexture;

			/*
			 * Render target
			 */
			GraphicsEngine::Direct3D::CERenderTarget m_renderTarget;

			/*
			 * Root Signature
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_rootSignature;

			/*
			 * Pipeline state object
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_pipelineState;
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_unlitPipelineState;

			D3D12_VIEWPORT m_viewPort;
			D3D12_RECT m_scissorRect;

			GraphicsEngine::Direct3D::Camera::CED3DCamera m_camera;

			struct alignas(16) CameraData {
				DirectX::XMVECTOR m_initialCamPos;
				DirectX::XMVECTOR m_initialCamRot;
			};

			CameraData* m_pAlignedCameraData;

			/*
			 * Define some lights
			 */
			std::vector<GraphicsEngine::Direct3D::Lighting::PointLight> m_pointLights;
			std::vector<GraphicsEngine::Direct3D::Lighting::SpotLight> m_spotLights;
		};
	}
}
