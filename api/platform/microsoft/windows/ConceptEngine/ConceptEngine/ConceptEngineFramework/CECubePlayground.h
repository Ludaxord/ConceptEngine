#pragma once
#include <d3d12.h>

#include "CEPlayground.h"
#include "CERenderTarget.h"

namespace Concept {
	namespace GraphicsEngine {
		namespace Direct3D {
			class CEPipelineStateObject;
			class CERootSignature;
			class CEIndexBuffer;
			class CEVertexBuffer;
			class CETexture;

			namespace Object {
				class CED3DCubeObject;
			}

			class CEScene;
			class CEGUI;
			class CESwapChain;
			class CEDevice;
		}
	}

	namespace GameEngine::Playground {
		class CECubePlayground : public CEPlayground {
		public:
			CECubePlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);

			bool LoadContent() override;
			void UnloadContent() override;
		protected:
			void OnUpdate(UpdateEventArgs& e) override;
			void OnRender() override;
			void OnGUI() override;
			void OnKeyPressed(KeyEventArgs& e) override;
			void OnKeyReleased(KeyEventArgs& e) override;
			void OnMouseMoved(MouseMotionEventArgs& e) override;
			void OnMouseWheel(MouseWheelEventArgs& e) override;
			void OnResize(ResizeEventArgs& e) override;
		private:
			/*
			 * DX12 Device
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEDevice> m_device;
			std::shared_ptr<GraphicsEngine::Direct3D::CESwapChain> m_swapChain;

			std::shared_ptr<GraphicsEngine::Direct3D::CETexture> m_depthTexture;
			std::shared_ptr<GraphicsEngine::Direct3D::CEVertexBuffer> m_vertexBuffer;
			std::shared_ptr<GraphicsEngine::Direct3D::CEIndexBuffer> m_indexBuffer;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_rootSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CEPipelineStateObject> m_pipelineStateObject;

			D3D12_VIEWPORT m_viewport;
			D3D12_RECT m_scissorRect;

			/*
			 * Some geometry to render
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::Object::CED3DCubeObject> m_cube;
		};
	}
}
