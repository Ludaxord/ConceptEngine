#pragma once
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
		class CERTXPlayground : public CEPlayground {

		public:
			CERTXPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
			virtual ~CERTXPlayground();

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
			D3D12_VIEWPORT m_viewPort;
			D3D12_RECT m_scissorRect;

			// Scale the HDR render target to a fraction of the window size.
			float m_RenderScale;
		};
	}

}
