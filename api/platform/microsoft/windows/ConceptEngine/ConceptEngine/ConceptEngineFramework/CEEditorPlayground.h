#pragma once
#include "CEPlayground.h"

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
		class CEEditorPlayground : public CEPlayground {
		public:
			CEEditorPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
			virtual ~CEEditorPlayground();

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
		};
	}
}
