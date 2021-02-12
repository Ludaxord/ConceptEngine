#pragma once
#include <d3d12.h>

#include "CED3DCamera.h"
#include "CED3DLight.h"
#include "CEPlayground.h"
#include "CERenderTarget.h"
#include "CEShaderResourceView.h"

namespace ConceptFramework {
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
		class CEHDRPlayground : public CEPlayground {
		public:
			CEHDRPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
			virtual ~CEHDRPlayground();

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
			           const GraphicsEngine::Direct3D::CERenderTarget& renderTarget);

			void RescaleHDRRenderTarget(float scale);
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
			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_cylinder;

			std::shared_ptr<GraphicsEngine::Direct3D::CEScene> m_skybox;

			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_defaultTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_directXTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_earthTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_monaLisaTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_GraceCathedralTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_GraceCathedralCubemap;
			std::shared_ptr<GraphicsEngine::Direct3D::CEShaderResourceView> m_GraceCathedralCubemapSRV;
			/*
			 * Render target
			 */
			GraphicsEngine::Direct3D::CERenderTarget m_renderTarget;
			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_HDRTexture;

			/*
			 * Root Signature
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_SkyboxSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_HDRRootSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_SDRRootSignature;

			/*
			 * Pipeline state object
			 */
			// Pipeline state object.
			// Skybox PSO
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_SkyboxPipelineState;
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_HDRPipelineState;
			// HDR -> SDR tone mapping PSO.
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_SDRPipelineState;
			// Unlit pixel shader (for rendering the light sources)
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_UnlitPipelineState;

			D3D12_VIEWPORT m_viewPort;
			D3D12_RECT m_scissorRect;

			GraphicsEngine::Direct3D::Camera::CED3DCamera m_camera;

			struct alignas(16) CameraData {
				DirectX::XMVECTOR m_initialCamPos;
				DirectX::XMVECTOR m_initialCamRot;
				float m_InitialFov;
			};

			CameraData* m_pAlignedCameraData;

			// Scale the HDR render target to a fraction of the window size.
			float m_RenderScale;
			/*
			 * Define some lights
			 */
			std::vector<GraphicsEngine::Direct3D::Lighting::HDRPointLight> m_pointLights;
			std::vector<GraphicsEngine::Direct3D::Lighting::HDRSpotLight> m_spotLights;
		};
	}
}
